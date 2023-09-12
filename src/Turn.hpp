#pragma once

struct Turn;

#include "Cards.hpp"
#include "Queen.hpp"
#include <list>
#include <variant>

struct CardMove {
  // consume playerIdx card
  CardMove(int playerIdx, int cardIdx, CardType type)
      : playerIdx(playerIdx), cardIdx(cardIdx){};
  CardMove(int playerIdx, int cardIdx, const Card &card)
      : playerIdx(playerIdx), cardIdx(cardIdx), card(card){};

  std::string toString();

  int playerIdx;
  int cardIdx;
  const Card card;
};

struct QueenMove {
  enum Type { KING, KNIGHT, POTION };
  // either knight or king move
  QueenMove(int playerIdx, int targetIdx, int cardIdx, Type type, Queen queen)
      : playerIdx(playerIdx), targetIdx(targetIdx), cardIdx(cardIdx),
        type(type), queen(queen){};

  std::string toString();

  int playerIdx;
  int targetIdx;
  int cardIdx;
  Type type;
  Queen queen;
};

using Move = std::variant<CardMove, QueenMove>;

std::string getMoveString(Move v);

struct Turn {
  Turn() : valid(false){};
  Turn(std::list<Move> m, bool valid) : moves(std::move(m)), valid(valid){};
  inline operator bool() const { return valid; }

  std::string toString();

  std::list<Move> moves;
  bool valid;
};