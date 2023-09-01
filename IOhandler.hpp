#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <functional>
#include <iostream>
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
  virtual std::string read(int player) = 0;
  virtual void write(int player, std::string str) = 0;
  virtual void write_all(std::string str) = 0;
};

class IOStreamHandler : public IOHandler {
public:
  virtual std::string read(int player) {
    std::string tmp;
    std::cin >> tmp;
    return tmp;
  }
  virtual void write(int player, std::string str) { std::cout << str; }
  virtual void write_all(std::string str) { std::cout << str; }
};

class PlayerSocket : public std::enable_shared_from_this<PlayerSocket> {
public:
  PlayerSocket(tcp::socket s) : socket(std::move(s)), timer_(s.get_executor()) {
    std::cout << "new user!" << std::endl;
    lastVal = {};
    timer_.expires_at(std::chrono::steady_clock::time_point::max());
  }

  void start() {
    co_spawn(
        socket.get_executor(),
        [self = shared_from_this()] { return self->read_from_socket(); },
        detached);
  }

  auto &get_socket() { return socket; }

  awaitable<std::string> read() {
    lastVal = {};
    // std::size_t n = boost::asio::read_until(
    //     playersSocket[player], boost::asio::dynamic_buffer(msg, 1024), "\n");
    while (!lastVal.has_value()) {
      boost::system::error_code ec;
      co_await timer_.async_wait(redirect_error(use_awaitable, ec));
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
        timer_.cancel_one();
        read_msg.erase(0, n);
      }
    } catch (std::exception &) {
    }
  }

  // awaitable<void> write_to_socket()

  tcp::socket socket;
  std::optional<std::string> lastVal;
  boost::asio::steady_timer timer_;

  std::mutex m_mtx;
  std::condition_variable m_cv;
};

class IOSocketHandler : public IOHandler {
public:
  IOSocketHandler(boost::asio::io_context &context)
      : new_player_timer(context) {
    new_player_timer.expires_at(std::chrono::steady_clock::time_point::max());
  };

  awaitable<void> acceptNewUsers(tcp::acceptor acceptor) {

    for (;;) {
      players.emplace_back(std::make_shared<PlayerSocket>(
          co_await acceptor.async_accept(use_awaitable)));

      auto &newPlayer = players.back();
      newPlayer->start(); // after constructor to get first shared_ptr

      new_player_timer.cancel();
    }
  }

  awaitable<void> reader(tcp::socket &sock, std::optional<std::string> &dest) {
    try {
      for (std::string read_msg;;) {
        std::size_t n = co_await boost::asio::async_read_until(
            sock, boost::asio::dynamic_buffer(read_msg, 1024), "\n",
            use_awaitable);

        dest = read_msg;

        std::cout << read_msg << std::endl;
        read_msg.erase(0, n);
      }
    } catch (std::exception &) {
    }
  };

  awaitable<void> await_num_of_players(int count) {
    while (players.size() < count) {
      boost::system::error_code ec;
      co_await new_player_timer.async_wait(redirect_error(use_awaitable, ec));
    }
  }

  virtual std::string read(int player) {
    // this is impossible, need to thread
    auto a = players[player]->read();
    return a.await_resume();
  }

  virtual void write(int player, std::string str) { std::cout << str; }
  virtual void write_all(std::string str) { std::cout << str; }

  std::vector<std::shared_ptr<PlayerSocket>> players;
  boost::asio::steady_timer new_player_timer;

  // std::vector<std::optional<std::string>> lastVal;
};