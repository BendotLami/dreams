#pragma once

#include <variant>

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

char printCard(const Card &c);