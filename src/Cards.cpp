#include "Cards.hpp"
#include "utils.hpp"
#include <sstream>
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
             [](auto c) { return "?"; }

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
             [](auto c) { return "?"; }

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
  // players[currentPlayer].addQueen(peekedQueen.value());

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
  while (!hasQueens) {
    msg << "Insert which player you want to attack: \n";
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
  while (!hasQueens) {
    msg << "Insert which player you want to attack: \n";
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
