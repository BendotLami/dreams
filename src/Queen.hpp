#pragma once

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

class QueenFactory {
public:
  static std::vector<Queen> getQueens();
};