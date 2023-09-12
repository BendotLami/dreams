#pragma once

#include "Cards.hpp"
#include <list>
#include <string>

class Deck {
  constexpr static int NumOfKings = 8;
  constexpr static int NumOfDragons = 3;
  constexpr static int NumOfWands = 3;
  constexpr static int NumOfKnights = 4;
  constexpr static int NumOfPotions = 4;
  constexpr static int NumOfJesters = 5;
  constexpr static int NumOfNumberCards = 40;
  constexpr static int TotalCards = NumOfKings + NumOfDragons + NumOfWands +
                                    NumOfKnights + NumOfPotions + NumOfJesters +
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

  Card peek_card();

private:
  void shuffle();

  std::list<Card> cards;
  int idx, nextShuffle;
};