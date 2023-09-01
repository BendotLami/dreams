#include "IOhandler.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/write.hpp>
#include <iostream>

using boost::asio::co_spawn;
using boost::asio::redirect_error;
awaitable<std::string> IOStreamHandler::read(int player) {
  std::string tmp;
  std::cin >> tmp;
  co_return tmp;
}
void IOStreamHandler::write(int player, std::string str) { std::cout << str; }
void IOStreamHandler::write_all(std::string str) { std::cout << str; }
PlayerSocket::PlayerSocket(tcp::socket s)
    : socket(std::move(s)), timer_read(s.get_executor()),
      timer_write(s.get_executor()) {
  std::cout << "new user!" << std::endl;
  timer_read.expires_at(std::chrono::steady_clock::time_point::max());
  timer_write.expires_at(std::chrono::steady_clock::time_point::max());
  lastVal = {};
}
void PlayerSocket::start() {
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
awaitable<std::string> PlayerSocket::read() {
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
awaitable<void> PlayerSocket::read_from_socket() {
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
void PlayerSocket::write(std::string msg) {
  write_msg_.push_back(msg);
  timer_write.cancel_one();
}
awaitable<void> PlayerSocket::write_to_socket() {
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
void PlayerSocket::stop() {
  if (socket.is_open())
    socket.close();
}
IOSocketHandler::IOSocketHandler(boost::asio::io_context &context)
    : context(context), new_player_timer(context) {
  new_player_timer.expires_at(std::chrono::steady_clock::time_point::max());
};
awaitable<void> IOSocketHandler::acceptNewUsers(tcp::acceptor act,
                                                int num_players) {
  try {
    acceptor.emplace(std::move(act));
    tcp::acceptor &acptPtr = *acceptor;
    for (; num_players > 0; num_players--) {
      players.emplace_back(std::make_shared<PlayerSocket>(
          co_await acptPtr.async_accept(use_awaitable)));

      auto &newPlayer = players.back();
      newPlayer->start(); // after constructor to get first shared_ptr

      new_player_timer.cancel();
    }
  } catch (std::exception &e) {
    throw e;
  }
}
awaitable<std::string> IOSocketHandler::read(int player) {
  // this is impossible, need to thread
  auto a = co_await players[player]->read();
  co_return a;
}
void IOSocketHandler::write(int player, std::string str) {
  players[player]->write(str);
}
void IOSocketHandler::write_all(std::string str) {
  for (const auto &p : players)
    p->write(str);
}
void IOSocketHandler::stop() {
  for (auto &p : players) {
    p->stop();
  }
}
