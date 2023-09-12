#pragma once

#include <optional>
#include <string>
#include <vector>

class Queen {
public:
  enum Type {
    Cat = 0,
    Dog,
    Roses,
    Regular,
  };

  Queen(int points, Type type);

  std::string toString() const;

  Type getType();

private:
  int points;
  Type type;
};

using GameQueen = std::pair<Queen, bool>;
using Queens = std::vector<GameQueen>;

class QueenFactory {
public:
  static Queens getQueens();
};

std::optional<Queen> peekQueen(const Queens &queens, int idx);

std::string queensString(const Queens &queens, bool debug);
std::string queensString(const Queens &queens);