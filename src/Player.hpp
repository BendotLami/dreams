#pragma once

#include <algorithm>
#include <list>
#include <variant>

class Player;
using Players = std::vector<Player>;

#include "Cards.hpp"
#include "Queen.hpp"

class Player {

  template <typename T> bool hasType();

public:
  Player(std::list<Card> &&cards);
  //   Player(std::list<Card> cards) : cards(cards){};

  const std::list<Card> &getCards();

  std::string printQueens() const;

  int getQueenCount() const;

  std::string printCards() const;

  std::string toString() const;

  void addQueen(Queen q);

  Queen removeQueen(int idx);

  Card peekCard(int idx);
  Queen peekQueen(int idx);

  // For now, multiple cards is multiple calls
  Card playCard(int idx, Card nextCard);

  bool hasPotion();
  bool hasWand();
  bool hasKnight();
  bool hasDragon();
  bool hasKing();

  inline friend std::ostream &operator<<(std::ostream &os, const Player &p) {
    os << p.toString();
    return os;
  }

  template <typename T> inline int find() {
    auto it = cards.begin();
    int i = 0;
    while (it != cards.end()) {
      if (std::holds_alternative<T>(*it))
        return i;
      it++;
      i++;
    }
    return -1;
  };

private:
  std::list<Queen> queens;
  std::list<Card> cards;
};

using Players = std::vector<Player>;