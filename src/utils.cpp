#include "utils.hpp"

awaitable<char> readInput(int playerIdx, std::string msg, IOHandler &io_handler,
                          std::function<bool(int)> predicate) {
  auto validateInput = [&predicate](const std::string &input) {
    if (input.size() != 1 && input.size() != 2)
      return false;
    unsigned char inputIdx = input[0] - '0';
    return predicate(inputIdx);
  };

  std::string input;
  do {
    io_handler.write(playerIdx, msg);
    input = co_await io_handler.read(playerIdx);
  } while (!validateInput(input));
  co_return input[0] - '0';
}