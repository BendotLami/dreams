#include "Cards.hpp"
#include "Queen.hpp"
#include <algorithm>
#include <list>
#include <variant>

#pragma once

class Player {
  template <typename T> bool hasType() {
    return std::find_if(cards.begin(), cards.end(), [](const Card &c) {
             return std::holds_alternative<T>(c);
           }) != cards.end();
  }

public:
  Player(std::list<Card> &&cards) : cards(cards){};
  //   Player(std::list<Card> cards) : cards(cards){};

  const std::list<Card> &getCards() { return cards; }

  std::string printQueens() const {
    std::string s;
    s.append("Queens: ");
    auto it = queens.begin();
    for (int i = 0; i < queens.size(); i++) {
      s.append(std::to_string(i));
      s.append(": ");
      s.append(it->toString());
      s.push_back(' ');
      ++it;
    }
    s.pop_back();
    return s;
  }

  int getQueenCount() const { return queens.size(); }

  std::string printCards() const {

    std::string s;
    s.append("Cards: ");
    auto it = cards.begin();
    int cardSize = cards.size();
    for (int i = 0; i < cardSize; i++) {
      s.append(std::to_string(i));
      s.append(": ");
      s.push_back(printCard(*it));
      s.push_back(' ');
      ++it;
    }
    s.pop_back();
    return s;
  }

  std::string toString() const {
    std::string s;
    s.append(printCards());
    s.push_back('\n');
    s.append(printQueens());
    s.push_back('\n');
    return s;
  }

  void addQueen(Queen q) { this->queens.emplace_back(std::move(q)); }

  Queen removeQueen(int idx) {
    auto it = queens.begin();
    std::advance(it, idx);
    Queen tmp = *it;
    queens.erase(it);
    return tmp;
  }

  Card peekCard(int idx) {
    auto it = cards.begin();
    std::advance(it, idx);
    return *it;
  }

  // For now, multiple cards is multiple calls
  Card playCard(int idx, Card nextCard) {
    auto it = cards.begin();
    std::advance(it, idx);
    std::swap(nextCard, *it);
    return nextCard;
  }

  bool hasPotion() { return hasType<Potion>(); }
  bool hasWand() { return hasType<Wand>(); }
  bool hasKnight() { return hasType<Knight>(); }
  bool hasDragon() { return hasType<Dragon>(); }
  bool hasKing() { return hasType<King>(); }

  friend std::ostream &operator<<(std::ostream &os, const Player &p) {
    os << p.toString();
    return os;
  }

  template <typename T> int find() {
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