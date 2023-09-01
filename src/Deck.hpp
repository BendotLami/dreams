#pragma once

#include "Cards.hpp"
#include <list>
#include <string>

class Deck {
  constexpr static int NumOfKings = 8;
  constexpr static int NumOfDragons = 8;
  constexpr static int NumOfWands = 8;
  constexpr static int NumOfKnights = 8;
  constexpr static int NumOfPotions = 8;
  constexpr static int NumOfNumberCards = 40;
  constexpr static int TotalCards = NumOfKings + NumOfDragons + NumOfWands +
                                    NumOfKnights + NumOfPotions +
                                    NumOfNumberCards;

public:
  Deck();

  std::string toString() const;

  inline friend std::ostream &operator<<(std::ostream &os, const Deck &d) {
    os << d.toString();
    return os;
  }

  Card pop_card();

  std::list<Card> pop_cards(int count);

  void push_card(Card c);

private:
  void shuffle();

  std::list<Card> cards;
  int idx, nextShuffle;
};