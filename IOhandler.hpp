#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/detail/throw_error.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/detail/errc.hpp>
#include <boost/system/system_error.hpp>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#pragma once

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::redirect_error;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

class IOHandler {
public:
  virtual awaitable<std::string> read(int player) = 0;
  virtual void write(int player, std::string str) = 0;
  virtual void write_all(std::string str) = 0;
};

class IOStreamHandler : public IOHandler {
public:
  virtual awaitable<std::string> read(int player) {
    std::string tmp;
    std::cin >> tmp;
    co_return tmp;
  }
  virtual void write(int player, std::string str) { std::cout << str; }
  virtual void write_all(std::string str) { std::cout << str; }
};

class PlayerSocket : public std::enable_shared_from_this<PlayerSocket> {
public:
  PlayerSocket(tcp::socket s)
      : socket(std::move(s)), timer_read(s.get_executor()),
        timer_write(s.get_executor()) {
    std::cout << "new user!" << std::endl;
    timer_read.expires_at(std::chrono::steady_clock::time_point::max());
    timer_write.expires_at(std::chrono::steady_clock::time_point::max());
    lastVal = {};
  }

  void start() {
    co_spawn(
        socket.get_executor(),
        [self = shared_from_this()] { return self->read_from_socket(); },
        [](std::exception_ptr e) {
          throw boost::asio::error::basic_errors::connection_aborted;
        });

    co_spawn(
        socket.get_executor(),
        [self = shared_from_this()] { return self->write_to_socket(); },
        [](std::exception_ptr e) {
          throw boost::asio::error::basic_errors::connection_aborted;
        });
  }

  auto &get_socket() { return socket; }

  awaitable<std::string> read() {
    lastVal = {};
    // std::size_t n = boost::asio::read_until(
    //     playersSocket[player], boost::asio::dynamic_buffer(msg, 1024), "\n");
    while (!lastVal.has_value()) {
      boost::system::error_code ec;
      co_await timer_read.async_wait(redirect_error(use_awaitable, ec));
    }

    std::string msg = *lastVal;
    std::cout << "player read: " << msg << std::endl;

    co_return msg;
  }

  awaitable<void> read_from_socket() {
    try {
      for (std::string read_msg;;) {
        std::size_t n = co_await boost::asio::async_read_until(
            socket, boost::asio::dynamic_buffer(read_msg, 1024), "\n",
            use_awaitable);

        std::cout << read_msg << std::endl;
        lastVal = read_msg;
        timer_read.cancel_one();
        read_msg.erase(0, n);
      }
    } catch (std::exception &c) {
      throw c;
    }
  }

  void write(std::string msg) {
    write_msg_.push_back(msg);
    timer_write.cancel_one();
  }

  awaitable<void> write_to_socket() {
    try {
      while (socket.is_open()) {
        if (write_msg_.empty()) {
          boost::system::error_code ec;
          co_await timer_write.async_wait(redirect_error(use_awaitable, ec));
        } else {
          co_await boost::asio::async_write(
              socket, boost::asio::buffer(write_msg_.front()), use_awaitable);
          write_msg_.pop_front();
        }
      }
    } catch (std::exception &c) {
      throw c;
    }
  }

  void stop() {
    if (socket.is_open())
      socket.close();
  }

  tcp::socket socket;
  std::optional<std::string> lastVal;
  boost::asio::steady_timer timer_read;
  std::deque<std::string> write_msg_;
  boost::asio::steady_timer timer_write;
};

class IOSocketHandler : public IOHandler {
public:
  IOSocketHandler(boost::asio::io_context &context)
      : new_player_timer(context) {
    new_player_timer.expires_at(std::chrono::steady_clock::time_point::max());
  };

  awaitable<void> acceptNewUsers(tcp::acceptor act) {
    try {
      acceptor.emplace(std::move(act));
      tcp::acceptor &acptPtr = *acceptor;
      for (;;) {
        players.emplace_back(std::make_shared<PlayerSocket>(
            co_await acptPtr.async_accept(use_awaitable)));

        auto &newPlayer = players.back();
        newPlayer->start(); // after constructor to get first shared_ptr

        new_player_timer.cancel();
      }
    } catch (boost::system::system_error &e) {
      if (e.code() == boost::asio::error::operation_aborted)
        ;
      else
        throw e;
    } catch (std::exception &e) {
      throw e;
    }
  }

  awaitable<void> await_num_of_players(int count) {
    while (players.size() < count) {
      boost::system::error_code ec;
      co_await new_player_timer.async_wait(redirect_error(use_awaitable, ec));
    }

    if (acceptor.has_value()) {
      acceptor->close();
      acceptor.reset();
    }
  }

  virtual awaitable<std::string> read(int player) {
    // this is impossible, need to thread
    auto a = co_await players[player]->read();
    co_return a;
  }

  virtual void write(int player, std::string str) {
    players[player]->write(str);
  }
  virtual void write_all(std::string str) {
    for (const auto &p : players)
      p->write(str);
  }

  void stop() {
    for (auto &p : players) {
      p->stop();
    }
  }

  std::vector<std::shared_ptr<PlayerSocket>> players;
  boost::asio::steady_timer new_player_timer;
  std::optional<tcp::acceptor> acceptor;
};