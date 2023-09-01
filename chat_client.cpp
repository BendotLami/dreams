#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <deque>
#include <iostream>
#include <memory>
#include <thread>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::redirect_error;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

static std::deque<std::string> write_msg_;

awaitable<void> writer(tcp::socket &sock, boost::asio::steady_timer &timer) {
  try {
    while (sock.is_open()) {
      if (write_msg_.empty()) {
        boost::system::error_code ec;
        co_await timer.async_wait(redirect_error(use_awaitable, ec));
      } else {
        co_await boost::asio::async_write(
            sock, boost::asio::buffer(write_msg_.front()), use_awaitable);
        write_msg_.pop_front();
      }
    }
  } catch (std::exception &) {
  }
}

awaitable<void> reader(tcp::socket &sock) {
  try {
    for (std::string read_msg;;) {
      std::size_t n = co_await boost::asio::async_read_until(
          sock, boost::asio::dynamic_buffer(read_msg, 1024), "\n",
          use_awaitable);

      std::cout << read_msg.substr(0, n);
      read_msg.erase(0, n);
    }
  } catch (std::exception &) {
  }
}

awaitable<void> read_cin(std::deque<std::string> &q,
                         boost::asio::steady_timer &timer) {
  try {
    for (;;) {
      std::string tmp;
      std::getline(std::cin, tmp);
      tmp.push_back('\n');
      q.push_back(tmp);
      timer.cancel_one();
    }
  } catch (std::exception &) {
    return awaitable<void>();
  }
  return awaitable<void>();
}

int main(int argc, char *argv[]) {
  try {
    if (argc < 2) {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }
    unsigned short port = std::atoi(argv[1]);

    boost::asio::io_context context(1);

    boost::asio::steady_timer timer(context);
    timer.expires_at(std::chrono::steady_clock::time_point::max());

    auto sock = tcp::socket(context);
    sock.connect(
        tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), port));

    boost::asio::signal_set signals(context, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) {
      sock.close();
      context.stop();
    });

    co_spawn(
        context, [&sock] { return reader(sock); },
        [](std::exception_ptr e) {
          throw boost::asio::error::basic_errors::connection_aborted;
        });

    co_spawn(
        context, [&timer, &sock] { return writer(sock, timer); },
        [](std::exception_ptr e) {
          throw boost::asio::error::basic_errors::connection_aborted;
        });
    ;

    std::thread thread(read_cin, std::ref(write_msg_), std::ref(timer));
    thread.detach();

    context.run();
  } catch (boost::asio::error::basic_errors &e) {
    std::cout << "Server closed" << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}