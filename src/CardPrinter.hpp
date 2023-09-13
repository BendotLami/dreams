#include "Queen.hpp"
#include <array>
#include <list>
#include <string>

static constexpr int CARD_WIDTH = 7;
static constexpr int CARD_HEIGHT = 5;

using PrettyCardType = std::array<std::string, CARD_HEIGHT>;
using PrettyCards = std::list<PrettyCardType>;

PrettyCards appendSleepyQueen(PrettyCards &pcard);
PrettyCards appendQueen(PrettyCards &pcard, const Queen &q);
PrettyCards appendEmptyCard(PrettyCards &pcard);

PrettyCards appendNumberCard(PrettyCards &pcard, char i);

PrettyCards appendKingCard(PrettyCards &pcard);
PrettyCards appendKnightCard(PrettyCards &pcard);
PrettyCards appendPotionCard(PrettyCards &pcard);
PrettyCards appendWandCard(PrettyCards &pcard);
PrettyCards appendDragonCard(PrettyCards &pcard);
PrettyCards appendJesterCard(PrettyCards &pcard);

std::string prettyToString(PrettyCards pcard, int linewidth = 255);