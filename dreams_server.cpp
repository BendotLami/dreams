#include "Game.hpp"
#include "IOhandler.hpp"
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <vector>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

// IOSocketHandler io_socket;

awaitable<void> playGame(IOSocketHandler &io_handler) {
  co_await io_handler.await_num_of_players(2);
  Game g(2, io_handler);
  int i = 0;
  while (true) {
    co_await g.PlayTurn(i);
    i = (i + 1) % 2;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: dreams_server <port>\n";
    return 1;
  }
  unsigned short port = std::atoi(argv[1]);

  std::cout << "starting" << std::endl;

  boost::asio::io_context io_context(1);
  IOSocketHandler io_socket{io_context};

  co_spawn(
      io_context,
      io_socket.acceptNewUsers(tcp::acceptor(io_context, {tcp::v4(), port})),
      detached);

  co_spawn(io_context, playGame(io_socket), detached);

  boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
  signals.async_wait([&](auto, auto) { io_context.stop(); });

  io_context.run();

  // IOStreamHandler ios;
  // Game g(3, ios);
  // int i = 0;
  // while (true) {
  //   g.PlayTurn(i);
  //   i = (i + 1) % 3;
  // }

  return 0;
}