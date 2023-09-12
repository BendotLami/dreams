#pragma once

#include "IOhandler.hpp"
#include <boost/asio/awaitable.hpp>
#include <string>
#include <variant>

using boost::asio::awaitable;

enum CardType { KING, DRAGON, WAND, KNIGHT, POTION, NUMBER };

class King {};
class Dragon {};
class Wand {};
class Knight {};
class Potion {};

class Number {
public:
  Number(int value) : value(value){};
  int getValue() { return value; };

private:
  int value;
};

using Card = std::variant<King, Dragon, Wand, Knight, Potion, Number>;

#include "Player.hpp"
#include "Turn.hpp"

template <typename T> bool isType(const Card &c) {
  return std::holds_alternative<T>(c);
}

CardType getType(const Card &c);

char printCard(const Card &c);
std::string printFullCard(const Card &c);

awaitable<Turn> playKing(const Players &players, const Queens &queens,
                         IOHandler &io_handler, int currentPlayer);

awaitable<Turn> playKnight(const Players &players, const Queens &queens,
                           IOHandler &io_handler, int currentPlayer);