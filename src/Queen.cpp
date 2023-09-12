#include "Queen.hpp"

Queen::Queen(int points, Type type) : points(points), type(type) {}
std::string Queen::toString() const {
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
Queens QueenFactory::getQueens() {
  Queens q;
  q.emplace_back(std::make_pair(Queen(15, Queen::Type::Cat), true));
  q.emplace_back(std::make_pair(Queen(15, Queen::Type::Dog), true));
  q.emplace_back(std::make_pair(Queen(5, Queen::Type::Roses), true));
  for (int i = 0; i < 9; i++)
    q.emplace_back(std::make_pair(Queen(10, Queen::Type::Regular), true));
  return q;
}

Queen::Type Queen::getType() { return type; };

std::optional<Queen> peekQueen(const Queens &queens, int idx) {
  if (queens[idx].second == false)
    return {};
  return queens[idx].first;
}

std::string queensString(const Queens &queens, bool debug) {
  std::string msg;
  int i = 0;
  for_each(queens.begin(), queens.end(), [&msg, &i, debug](GameQueen a) {
    std::string queenRepr = a.second ? (debug ? a.first.toString() : "Q") : "_";
    msg.append(std::to_string(i++));
    msg.append(": ");
    msg.append(queenRepr);
    msg.push_back(' ');
  });
  msg.pop_back();

  return msg;
}

std::string queensString(const Queens &queens) {
  return queensString(queens, false);
}