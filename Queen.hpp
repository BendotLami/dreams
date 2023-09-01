#include <string>
#include <vector>

#pragma once

class Queen {
public:
  enum Type {
    Cat = 0,
    Dog,
    Roses,
    Regular,
  };

  Queen(int points, Type type) : points(points), type(type) {}

  std::string toString() const {
    switch (type) {
    case Type::Cat:
      return "Cat";
    case Type::Dog:
      return "Dog";
    case Type::Regular:
      return "Regular";
    case Type::Roses:
      return "Roses";
    default:
      return "???";
    }
  }

private:
  int points;
  Type type;
};

class QueenFactory {
public:
  static std::vector<Queen> getQueens() {
    std::vector<Queen> q;
    q.emplace_back(15, Queen::Type::Cat);
    q.emplace_back(15, Queen::Type::Dog);
    q.emplace_back(5, Queen::Type::Roses);
    for (int i = 0; i < 9; i++)
      q.emplace_back(10, Queen::Type::Regular);
    return q;
  }
};