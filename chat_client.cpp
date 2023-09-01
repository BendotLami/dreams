#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
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

// awaitable<void> reader() {
//   try {
//     for (std::string read_msg;;) {
//       std::size_t n = co_await boost::asio::async_read_until(
//           socket_, boost::asio::dynamic_buffer(read_msg, 1024), "\n",
//           use_awaitable);

//       room_.deliver(read_msg.substr(0, n));
//       read_msg.erase(0, n);
//     }
//   } catch (std::exception &) {
//     stop();
//   }
// }

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

      std::cout << read_msg << std::endl;
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
      // std::cin >> tmp;
      std::getline(std::cin, tmp);
      // std::cout << "read: " << tmp << std::endl;
      tmp.push_back('\n');
      q.push_back(tmp);
      timer.cancel_one();
    }
  } catch (std::exception &) {
    return awaitable<void>();
  }
  return awaitable<void>();
}

std::shared_ptr<tcp::socket> sock;

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

    sock = std::make_shared<tcp::socket>(tcp::socket(context));
    sock->connect(
        tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), port));
    if (!sock->is_open())
      std::cout << "func this sheet" << std::endl;

    boost::asio::signal_set signals(context, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) {
      std::cout << "closing" << std::endl;
      sock->close();
      context.stop();
    });

    co_spawn(
        context, [] { return reader(*sock); }, detached);

    co_spawn(
        context, [&timer] { return writer(*sock, timer); }, detached);

    std::thread thread(read_cin, std::ref(write_msg_), std::ref(timer));
    thread.detach();

    // boost::asio::post(context, [&sock]() { sock.close(); });

    context.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}