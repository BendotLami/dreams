#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <deque>
#include <string>
#include <vector>

using boost::asio::awaitable;
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
  virtual awaitable<std::string> read(int player);
  virtual void write(int player, std::string str);
  virtual void write_all(std::string str);
};

class PlayerSocket : public std::enable_shared_from_this<PlayerSocket> {
public:
  PlayerSocket(tcp::socket s);

  void start();

  awaitable<std::string> read();

  awaitable<void> read_from_socket();

  void write(std::string msg);

  awaitable<void> write_to_socket();

  void stop();

private:
  tcp::socket socket;
  std::optional<std::string> lastVal;
  boost::asio::steady_timer timer_read;
  std::deque<std::string> write_msg_;
  boost::asio::steady_timer timer_write;
};

class IOSocketHandler : public IOHandler {
public:
  IOSocketHandler(boost::asio::io_context &context);

  awaitable<void> acceptNewUsers(tcp::acceptor act, int num_players);

  virtual awaitable<std::string> read(int player);

  virtual void write(int player, std::string str);
  virtual void write_all(std::string str);

  void stop();

  boost::asio::io_context &get_context() { return context; }

private:
  std::vector<std::shared_ptr<PlayerSocket>> players;
  boost::asio::steady_timer new_player_timer;
  std::optional<tcp::acceptor> acceptor;
  boost::asio::io_context &context;
};