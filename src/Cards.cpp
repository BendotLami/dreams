#include "Cards.hpp"
#include "overload.hpp"

static auto printCardVisitor =
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

char printCard(const Card &c) { return std::visit(printCardVisitor, c); }