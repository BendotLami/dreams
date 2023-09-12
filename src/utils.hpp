#pragma once

#include "IOhandler.hpp"
#include <boost/asio/awaitable.hpp>

using boost::asio::awaitable;

template <typename... Ts> struct Overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;

awaitable<char> readInput(int playerIdx, std::string msg, IOHandler &io_handler,
                          std::function<bool(int)> predicate);
