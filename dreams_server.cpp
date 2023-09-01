#include "./src/Game.hpp"
#include "./src/IOhandler.hpp"
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <vector>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::ip::tcp;

// IOSocketHandler io_socket;

awaitable<void> playGame(IOSocketHandler &io_handler, int num_players,
                         unsigned short port) {
  co_await io_handler.acceptNewUsers(
      tcp::acceptor(io_handler.get_context(), {tcp::v4(), port}), num_players);
  Game g(num_players, io_handler);
  int i = 0;
  while (true) {
    std::cout << "oh its on" << std::endl;
    co_await g.PlayTurn(i);
    i = (i + 1) % 2;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2 && argc != 3) {
    std::cerr << "Usage: dreams_server <port> (<num of players>)\n";
    return 1;
  }
  unsigned short port = std::atoi(argv[1]);
  unsigned short num_players = 2;
  if (argc > 2)
    num_players = std::atoi(argv[2]);

  std::cout << "starting" << std::endl;

  boost::asio::io_context io_context(1);
  IOSocketHandler io_socket{io_context};

  co_spawn(io_context, playGame(io_socket, num_players, port), detached);

  boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
  signals.async_wait([&](auto, auto) { io_context.stop(); });

  try {
    io_context.run();
  } catch (boost::asio::error::basic_errors &e) {
    std::cout << "closing" << std::endl;
    io_socket.stop();
  }

  return 0;
}