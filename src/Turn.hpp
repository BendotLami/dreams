#pragma once

#include <list>
#include <variant>

struct CardMove {
  // consume playerIdx card
  CardMove(int playerIdx, int cardIdx)
      : playerIdx(playerIdx), cardIdx(cardIdx){};

  int playerIdx;
  int cardIdx;
};

struct QueenMove {
  enum Type { KING, KNIGHT, POTION };
  // either knight or king move
  QueenMove(int playerIdx, int targetIdx, int cardIdx, Type type)
      : playerIdx(playerIdx), targetIdx(targetIdx), cardIdx(cardIdx),
        type(type){};

  int playerIdx;
  int targetIdx;
  int cardIdx;
  Type type;
};

using Move = std::variant<CardMove, QueenMove>;

struct Turn {
  Turn() : valid(false){};
  Turn(std::list<Move> m, bool valid) : moves(std::move(m)), valid(valid){};
  inline operator bool() const { return valid; }

  std::list<Move> moves;
  bool valid;
};