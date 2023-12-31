#include "Cards.hpp"
#include "utils.hpp"
#include <sstream>
#include <string>
#include <vector>

static char toHex(char i) {
  if (i > 15 || i < 0)
    return '?';
  return i == 10 ? 'A' + i - 10 : '0' + i;
}

static auto printCardVisitor =
    Overload{[](King c) { return 'K'; },
             [](Dragon c) { return 'd'; },
             [](Wand c) { return 'w'; },
             [](Knight c) { return 'k'; },
             [](Potion c) { return 'p'; },
             [](Number c) -> char { return toHex(c.getValue()); },
             [](Jester c) { return 'j'; },
             [](auto c) { return '?'; }

    };

static auto printFullCardVisitor =
    Overload{[](King c) { return std::string("King"); },
             [](Dragon c) { return std::string("Dragon"); },
             [](Wand c) { return std::string("Wand"); },
             [](Knight c) { return std::string("Knight"); },
             [](Potion c) { return std::string("Potion"); },
             [](Number c) {
               std::string s("Number ");
               s.append(std::to_string(c.getValue()));
               return s;
             },
             [](Jester c) { return std::string("Jester"); },
             [](auto c) { return std::string("?"); }

    };

char printCard(const Card &c) { return std::visit(printCardVisitor, c); }

std::string printFullCard(const Card &c) {
  return std::visit(printFullCardVisitor, c);
}

CardType getType(const Card &c) {
  auto getTypeVisitor = Overload{[](King c) { return CardType::KING; },
                                 [](Dragon c) { return CardType::DRAGON; },
                                 [](Wand c) { return CardType::WAND; },
                                 [](Knight c) { return CardType::KNIGHT; },
                                 [](Potion c) { return CardType::POTION; },
                                 [](Number c) { return CardType::NUMBER; },
                                 [](Jester c) { return CardType::JESTER; },
                                 [](auto c) { return "?"; }};

  return std::visit(getTypeVisitor, c);
}

awaitable<Turn> playKing(const Players &players, const Queens &queens,
                         IOHandler &io_handler, int currentPlayer) {
  std::stringstream msg;

  std::vector<int> range;
  for (int i = 0; i < queens.size(); i++)
    if (queens[i].second)
      range.push_back(i);

  if (range.size() == 0) {
    // std::cout << "No hidden queens available" << std::endl;
    io_handler.write(currentPlayer, "No hidden queens available");
    co_return Turn();
  }

  auto isQueenIdxValid = [&range](int i) {
    for (const auto j : range)
      if (i == j)
        return true;
    return false;
  };

  msg << "Hidden Queens:\n";
  msg << queensString(queens, true);
  msg << "\n"
      << "Insert chosen queen: \n";

  int input =
      co_await readInput(currentPlayer, msg.str(), io_handler, isQueenIdxValid);
  auto peekedQueen = peekQueen(queens, input);
  if (!peekedQueen.has_value())
    throw std::exception();

  Turn res =
      Turn({QueenMove(currentPlayer, -1, input, QueenMove::KING, *peekedQueen)},
           true);

  if (peekedQueen->getType() == Queen::Type::Roses) {
    Queens copy = queens;
    copy[input].second = false;
    Turn tmp = co_await playKing(players, copy, io_handler, currentPlayer);
    if (res.valid)
      res.moves.splice(res.moves.end(), tmp.moves);
  }

  co_return res;
}

awaitable<Turn> playKnight(const Players &players, const Queens &queens,
                           IOHandler &io_handler, int currentPlayer) {
  std::stringstream msg;

  if (!anyPlayerHasQueens(players)) {
    io_handler.write(currentPlayer, "No player have any queens.\n");
    co_return Turn();
  }

  bool hasQueens = false;
  int attackedPlayerIdx;
  for (int i = 0; i < players.size(); i++) {
    if (players[i].getQueenCount() <= 0)
      continue;
    msg << "Player " << std::to_string(i) << '\n';
    msg << players[i].printQueens();
  }
  msg << "Insert which player you want to attack: \n";
  while (!hasQueens) {
    attackedPlayerIdx = co_await readInput(
        currentPlayer, msg.str(), io_handler, [&players, currentPlayer](int i) {
          return i >= 0 && i < players.size() && i != currentPlayer;
        });
    hasQueens = players[attackedPlayerIdx].getQueenCount() > 0;
    if (!hasQueens)
      io_handler.write(currentPlayer, "Player does not have queens.\n");
  }
  auto &attackedPlayer = players[attackedPlayerIdx];

  msg.str(std::string());
  msg << attackedPlayer.printQueens() << '\n';
  msg << "Pick which queen you want to steal: \n";
  int queenIdx = co_await readInput(
      currentPlayer, msg.str(), io_handler, [&attackedPlayer](int i) {
        return i >= 0 && i < attackedPlayer.getQueenCount();
      });

  co_return Turn(
      {QueenMove(currentPlayer, attackedPlayerIdx, queenIdx, QueenMove::KNIGHT,
                 attackedPlayer.peekQueen(queenIdx))}, // TODO: might be bug
      true);
}

awaitable<Turn> playPotion(const Players &players, const Queens &queens,
                           IOHandler &io_handler, int currentPlayer) {
  std::stringstream msg;

  if (!anyPlayerHasQueens(players)) {
    io_handler.write(currentPlayer, "No player have any queens.");
    co_return Turn();
  }

  bool hasQueens = false;
  int attackedPlayerIdx;
  for (int i = 0; i < players.size(); i++) {
    if (players[i].getQueenCount() <= 0)
      continue;
    msg << "Player " << std::to_string(i) << '\n';
    msg << players[i].printQueens();
  }
  msg << "Insert which player you want to attack: \n";
  while (!hasQueens) {
    attackedPlayerIdx = co_await readInput(
        currentPlayer, msg.str(), io_handler, [&players, currentPlayer](int i) {
          return i >= 0 && i < players.size() && i != currentPlayer;
        });
    hasQueens = players[attackedPlayerIdx].getQueenCount() > 0;
    if (!hasQueens)
      io_handler.write(currentPlayer, "Player does not have queens.");
  }
  auto &attackedPlayer = players[attackedPlayerIdx];

  msg.str(std::string());
  msg << attackedPlayer.printQueens() << '\n';
  msg << "Pick which queen you want to poison: \n";
  int queenIdx = co_await readInput(
      currentPlayer, msg.str(), io_handler, [&attackedPlayer](int i) {
        return i >= 0 && i < attackedPlayer.getQueenCount();
      });

  co_return Turn(
      {QueenMove(currentPlayer, attackedPlayerIdx, queenIdx, QueenMove::POTION,
                 attackedPlayer.peekQueen(queenIdx))},
      true); // TODO: stopped in the middle
}

awaitable<Turn> playJester(const Players &players, const Queens &queens,
                           IOHandler &io_handler, Card nextCard,
                           int currentPlayer) {
  std::string msg;

  int number = std::visit(Overload{[](Number c) { return c.getValue(); },
                                   [](auto c) { return -1; }},
                          nextCard);

  msg.append("Next card in deck is ");
  msg.append(printFullCard(nextCard));
  msg.push_back('\n');

  io_handler.write_all(msg);

  if (number < 0) {
    // taken a special card, continue with a valid empty move
    // NOTE: assuming nextCard is a peek to the deck, meaning this card WILL be
    // taken by the player
    co_return Turn({}, true);
  }

  // taken a number, winnerPlayer should wake a queen and continue
  int winnerPlayer = (currentPlayer + number) % players.size();
  msg.clear();
  msg.append("Player ");
  msg.append(std::to_string(winnerPlayer));
  msg.append(" has won jester round. Currently choosing queen...\n");
  Turn kingTurn = co_await playKing(players, queens, io_handler, winnerPlayer);

  co_return kingTurn;
}
