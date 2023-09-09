#include "Cards.hpp"
#include "overload.hpp"

static char toHex(char i) {
  if (i > 15 || i < 0)
    return '?';
  return i == 10 ? 'A' + i - 10 : '0' + i;
}

static auto printCardVisitor =
    Overload{[](King c) { return 'K'; },
             [](Dragon c) { return 'd'; },
             [](Wand c) { return 'w'; },
             [](Knight c) { return 'k'; },
             [](Potion c) { return 'p'; },
             [](Number c) -> char { return toHex(c.getValue()); },
             [](auto c) { return "?"; }

    };

static auto printFullCardVisitor =
    Overload{[](King c) { return std::string("King"); },
             [](Dragon c) { return std::string("Dragon"); },
             [](Wand c) { return std::string("Wand"); },
             [](Knight c) { return std::string("Knight"); },
             [](Potion c) { return std::string("Potion"); },
             [](Number c) {
               std::string s("Number ");
               s.append(std::to_string(c.getValue()));
               return s;
             },
             [](auto c) { return "?"; }

    };

char printCard(const Card &c) { return std::visit(printCardVisitor, c); }

std::string printFullCard(const Card &c) {
  return std::visit(printFullCardVisitor, c);
}

CardType getType(const Card &c) {
  auto getTypeVisitor = Overload{[](King c) { return CardType::KING; },
                                 [](Dragon c) { return CardType::DRAGON; },
                                 [](Wand c) { return CardType::WAND; },
                                 [](Knight c) { return CardType::KNIGHT; },
                                 [](Potion c) { return CardType::POTION; },
                                 [](Number c) { return CardType::NUMBER; },
                                 [](auto c) { return "?"; }};

  return std::visit(getTypeVisitor, c);
}