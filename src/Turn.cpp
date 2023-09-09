#include "Turn.hpp"
#include "Cards.hpp"
#include "overload.hpp"
#include <string>

std::string CardMove::toString() {
  std::string res = "Player ";
  res.append(std::to_string(playerIdx));
  res.append(" played ");
  res.append(printFullCard(card));
  res.push_back('\n');
  return res;
}

std::string QueenMove::toString() {
  std::string res = "Queen ";
  res += queen.toString() + " index ";
  res += std::to_string(cardIdx) + " ";
  res += "From " +
         (type == Type::KING ? "Deck" : "player" + std::to_string(playerIdx));
  res +=
      " was taken to: " +
      (type == Type::POTION ? "Deck" : "player " + std::to_string(playerIdx));
  res += '\n';

  return res;
}

std::string getMoveString(Move v) {
  auto visitor = Overload{
      [](CardMove &c) { return c.toString(); },
      [](QueenMove &c) { return c.toString(); },
  };

  return std::visit(visitor, v);
}

std::string Turn::toString() {
  if (!valid)
    return "";
  std::string res;
  for (const auto &m : moves)
    res += getMoveString(m);
  return res;
}