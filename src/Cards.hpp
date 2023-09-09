#pragma once

#include <string>
#include <variant>

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

template <typename T> bool isType(const Card &c) {
  return std::holds_alternative<T>(c);
}

CardType getType(const Card &c);

char printCard(const Card &c);
std::string printFullCard(const Card &c);