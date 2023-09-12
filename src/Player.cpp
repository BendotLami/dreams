#include "Player.hpp"

Player::Player(std::list<Card> &&cards) : cards(cards){};
const std::list<Card> &Player::getCards() { return cards; }
std::string Player::printQueens() const {
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
int Player::getQueenCount() const { return queens.size(); }
std::string Player::printCards() const {

  std::string s;
  s.append("Cards: ");
  auto it = cards.begin();
  int cardSize = cards.size();
  for (int i = 0; i < cardSize; i++) {
    s.append(std::to_string(i));
    s.append(": ");
    s.append(printFullCard(*it));
    s.push_back(' ');
    ++it;
  }
  s.pop_back();
  return s;
}
std::string Player::toString() const {
  std::string s;
  s.append(printCards());
  s.push_back('\n');
  s.append(printQueens());
  s.push_back('\n');
  return s;
}
void Player::addQueen(Queen q) { this->queens.emplace_back(std::move(q)); }

Queen Player::removeQueen(int idx) {
  auto it = queens.begin();
  std::advance(it, idx);
  Queen tmp = *it;
  queens.erase(it);
  return tmp;
}
Card Player::peekCard(int idx) {
  auto it = cards.begin();
  std::advance(it, idx);
  return *it;
}

Queen Player::peekQueen(int idx) {
  auto it = queens.begin();
  std::advance(it, idx);
  return *it;
}

Card Player::playCard(int idx, Card nextCard) {
  auto it = cards.begin();
  std::advance(it, idx);
  std::swap(nextCard, *it);
  return nextCard;
}
template <typename T> bool Player::hasType() {
  return std::find_if(cards.begin(), cards.end(), [](const Card &c) {
           return std::holds_alternative<T>(c);
         }) != cards.end();
}
bool Player::hasPotion() { return hasType<Potion>(); }
bool Player::hasWand() { return hasType<Wand>(); }
bool Player::hasKnight() { return hasType<Knight>(); }
bool Player::hasDragon() { return hasType<Dragon>(); }
bool Player::hasKing() { return hasType<King>(); }
