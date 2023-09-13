#include "CardPrinter.hpp"
#include <iterator>
#include <utility>
// number  hidden queen  queen
// ┌─────┐┌─────┐┌─────┐┌─────┐
// │1    ││Q░░░░││Q    ││Q    │
// │     ││░░░░░││ Reg ││Roses│
// │    1││░░░░Q││    Q││    Q│
// └─────┘└─────┘└─────┘└─────┘

PrettyCards appendGeneralCard(PrettyCards &pcard, char c) {
  PrettyCardType card;
  card[0].append("┌─────┐");
  card[1].append("│" + std::string(1, c) + "    │");
  card[2].append("│     │");
  card[3].append("│    " + std::string(1, c) + "│");
  card[4].append("└─────┘");
  pcard.push_back(card);
  return pcard;
}

PrettyCards appendNumberCard(PrettyCards &pcard, char i) {
  return appendGeneralCard(pcard, '0' + i);
}

PrettyCards appendSleepyQueen(PrettyCards &pcard) {
  PrettyCardType card;
  card[0].append("┌─────┐");
  card[1].append("│Q░░░░│");
  card[2].append("│░░░░░│");
  card[3].append("│░░░░Q│");
  card[4].append("└─────┘");
  pcard.push_back(card);
  return pcard;
}

PrettyCards appendEmptyCard(PrettyCards &pcard) {
  PrettyCardType card;
  card[0].append("┌─────┐");
  card[1].append("│     │");
  card[2].append("│     │");
  card[3].append("│     │");
  card[4].append("└─────┘");
  pcard.push_back(card);
  return pcard;
}

PrettyCards appendQueen(PrettyCards &pcard, const Queen &q) {
  std::string type = "     ";
  switch (q.getType()) {
  case Queen::Type::Cat:
    type = " Cat ";
    break;
  case Queen::Type::Dog:
    type = " Dog ";
    break;
  case Queen::Type::Regular:
    type = " Reg ";
    break;
  case Queen::Type::Roses:
    type = "Roses";
    break;
  default:
    break;
  }

  PrettyCardType card;
  card[0].append("┌─────┐");
  card[1].append("│Q    │");
  card[2].append("|" + type + "|");
  card[3].append("│    Q│");
  card[4].append("└─────┘");
  pcard.push_back(card);
  return pcard;
}

PrettyCards appendKingCard(PrettyCards &pcard) {
  return appendGeneralCard(pcard, 'K');
}

PrettyCards appendKnightCard(PrettyCards &pcard) {
  return appendGeneralCard(pcard, 'k');
}

PrettyCards appendPotionCard(PrettyCards &pcard) {
  return appendGeneralCard(pcard, 'p');
}

PrettyCards appendWandCard(PrettyCards &pcard) {
  return appendGeneralCard(pcard, 'w');
}

PrettyCards appendDragonCard(PrettyCards &pcard) {
  return appendGeneralCard(pcard, 'd');
}

PrettyCards appendJesterCard(PrettyCards &pcard) {
  return appendGeneralCard(pcard, 'j');
}

std::string prettyToString(PrettyCards pcard, int linewidth) {
  std::string res = "";
  int numCards = pcard.size();
  int step = (linewidth / CARD_WIDTH);
  if (step <= 0)
    return "invalid pretty to string";

  auto pos = pcard.begin();
  int currLineCards = 0;
  int idx = 0;

  std::array<std::string, CARD_HEIGHT + 1> middleString;

  middleString[CARD_HEIGHT] = std::string(CARD_WIDTH / 2, ' ');
  while (pos != pcard.end()) {
    for (int i = 0; i < CARD_HEIGHT; i++)
      middleString[i] += (*pos)[i];
    middleString[CARD_HEIGHT] +=
        std::to_string(idx) + std::string(CARD_WIDTH - 1 - (idx / 10), ' ');

    currLineCards++;
    pos++;
    idx++;

    if (currLineCards >= step) {
      // flush
      for (int i = 0; i < CARD_HEIGHT + 1; i++) {
        res += middleString[i] + '\n';
        middleString[i].clear();
      }
      middleString[CARD_HEIGHT] = std::string(CARD_WIDTH / 2, ' ');
      currLineCards = 0;
    }
  }
  // if anything, flush again
  if (middleString[0].size())
    for (int i = 0; i < CARD_HEIGHT + 1; i++)
      res += middleString[i] + '\n';

  return res;
}
