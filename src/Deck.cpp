#include "Deck.hpp"
#include "Cards.hpp"
#include <random>
#include <vector>

Deck::Deck() {
  // cards.reserve(TotalCards);
  for (int i = 0; i < NumOfKings; i++)
    cards.emplace_back(King());
  for (int i = 0; i < NumOfDragons; i++)
    cards.emplace_back(Dragon());
  for (int i = 0; i < NumOfWands; i++)
    cards.emplace_back(Wand());
  for (int i = 0; i < NumOfKnights; i++)
    cards.emplace_back(Knight());
  for (int i = 0; i < NumOfPotions; i++)
    cards.emplace_back(Potion());
  for (int i = 0; i < NumOfNumberCards; i++)
    cards.emplace_back(Number(i / 4));

  shuffle();
  idx = 0;
}
std::string Deck::toString() const {
  std::string s;
  for (const auto &c : cards) {
    s.push_back(printCard(c));
    s.push_back(' ');
  }
  s.pop_back();
  return s;
}
Card Deck::pop_card() {
  if (idx >= nextShuffle) {
    shuffle();
  }
  Card temp = cards.front();
  cards.pop_front();
  idx++;
  return temp;
};
std::list<Card> Deck::pop_cards(int count) {
  std::list<Card> rtn;
  for (int i = 0; i < count; i++)
    rtn.push_back(pop_card());
  return rtn;
}
void Deck::push_card(Card c) { cards.push_back(c); }
void Deck::shuffle() {
  std::vector<Card> shuf(cards.size());
  std::copy(cards.begin(), cards.end(), shuf.begin());

  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(shuf.begin(), shuf.end(), g);

  std::copy(shuf.begin(), shuf.end(), cards.begin());

  nextShuffle = cards.size();
  idx = 0;
};