#include "overload.hpp"
#include <variant>

#pragma once

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

template <typename T>
static auto isType = [](const Card &c) { return std::holds_alternative<T>(c); };

auto printCardVisitor =
    Overload{[](King c) { return 'K'; },
             [](Dragon c) { return 'd'; },
             [](Wand c) { return 'w'; },
             [](Knight c) { return 'k'; },
             [](Potion c) { return 'p'; },
             [](Number c) -> char {
               return c.getValue() == 10 ? 'A' : '0' + c.getValue();
             },
             [](auto c) { return "?"; }

    };

auto printCard = [](const Card &c) { return std::visit(printCardVisitor, c); };