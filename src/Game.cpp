#include "Game.hpp"
#include "Cards.hpp"
#include "Queen.hpp"
#include "Turn.hpp"
#include "utils.hpp"
#include <iostream>
#include <list>
#include <random>
#include <ranges>
#include <sstream>
#include <variant>

void Game::initCards(int players) {
  for (int i = 0; i < players; i++) {
    this->players.push_back(deck.pop_cards(CardsPerPlayer));
  }
}
awaitable<char> Game::readInput(int playerIdx, std::string msg,
                                std::function<bool(int)> predicate) {
  return ::readInput(playerIdx, msg, io_handler, predicate);
}

Game::Game(int players, IOHandler &io_handler) : io_handler(io_handler) {
  initCards(players);
  queens = QueenFactory::getQueens();
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(queens.begin(), queens.end(), g);
};
std::string Game::getPlayerHand(int player) {
  if (player >= players.size())
    return "Error: No Such Player!";
  return players[player].toString();
}
std::string Game::playersString() {
  std::stringstream ss;
  for (int i = 0; i < players.size(); i++) {
    ss << "Player " << i << ":\n";
    ss << players[i] << "\n";
  }
  return ss.str();
}
void Game::printPlayers() { io_handler.write_all(playersString()); }

std::string Game::queensString(bool debug) {
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

std::optional<int> Game::handleDragon(int attackedIdx) {
  auto &player = players[attackedIdx];
  int dragonIdx = player.find<Dragon>();
  if (dragonIdx < 0)
    return {};

  return dragonIdx;
}

std::optional<int> Game::handleWand(int attackedIdx) {
  auto &player = players[attackedIdx];
  int wandIdx = player.find<Wand>();
  if (wandIdx < 0)
    return {};

  return wandIdx;
}

awaitable<Turn> Game::playKnight(int currentPlayer) {
  Turn turn = co_await ::playKnight(players, queens, io_handler, currentPlayer);

  if (!turn.valid)
    co_return Turn();

  int attackedPlayerIdx =
      std::visit(Overload{[](QueenMove q) { return q.targetIdx; },
                          [](auto _) { return -1; }},
                 turn.moves.front());
  if (attackedPlayerIdx < 0)
    co_return Turn();

  auto dragonIdx = handleDragon(attackedPlayerIdx);

  if (dragonIdx.has_value()) { // maybe later will wait for result of
                               // other player
    // poor baby, took your knight
    co_return Turn({CardMove(attackedPlayerIdx, *dragonIdx,
                             players[attackedPlayerIdx].peekCard(*dragonIdx))},
                   true);
  }

  co_return turn;
}

awaitable<Turn> Game::playKing(int currentPlayer) {
  return ::playKing(players, queens, io_handler, currentPlayer);
}

awaitable<Turn> Game::playPotion(int currentPlayer) {

  Turn turn = co_await ::playPotion(players, queens, io_handler, currentPlayer);

  if (!turn.valid)
    co_return Turn();

  int attackedPlayerIdx =
      std::visit(Overload{[](QueenMove q) { return q.targetIdx; },
                          [](auto _) { return -1; }},
                 turn.moves.front());

  auto wandIdx = handleWand(attackedPlayerIdx);

  if (wandIdx.has_value()) { // maybe later will wait for result of
                             // other player
    // poor baby, the princess is awake
    co_return Turn({CardMove(attackedPlayerIdx, *wandIdx,
                             players[attackedPlayerIdx].peekCard(*wandIdx))},
                   true);
  }

  co_return turn;
}

awaitable<void> Game::PlayTurn(int playerIdx) {
  // TODO: DEBUG ONLY:
  // this->printPlayers();
  // END TODO

  std::stringstream msg;
  auto &player = players[playerIdx];
  msg << "Player " << playerIdx << "\n";
  msg << player;
  msg << "Insert which card you want to play: \n";

  Turn res;
  int cardIdx;

  while (!res) {
    res = Turn();
    cardIdx = co_await readInput(playerIdx, msg.str(), [this](int i) {
      return i >= 0 && i < CardsPerPlayer;
    });

    // Card playedCard = player.playCard(cardIdx, deck.pop_card());
    Card peekedCard = player.peekCard(cardIdx);
    // std::cout << printCard(peekedCard) << std::endl;
    std::string str;
    str.append("You played ");
    str.append(printFullCard(peekedCard));
    str.push_back('\n');
    io_handler.write(playerIdx, str);

    res = co_await std::visit(
        Overload{[playerIdx](Number n) -> awaitable<Turn> {
                   co_return Turn({}, true);
                 },
                 [this, playerIdx](Knight k) { return playKnight(playerIdx); },
                 [this, playerIdx](King k) { return playKing(playerIdx); },
                 [this, playerIdx](Potion k) { return playPotion(playerIdx); },
                 [](Dragon _) -> awaitable<Turn> { co_return Turn({}, false); },
                 [](Wand _) -> awaitable<Turn> { co_return Turn({}, false); },
                 [](auto c) {
                   std::cerr << "errer: " << printCard(c) << std::endl;
                   return Turn({}, false);
                 }},
        peekedCard);

    res.moves.push_front({CardMove(playerIdx, cardIdx, peekedCard)});
  }

  commitTurn(res);
}

bool Game::commitTurn(const Turn &turn) {
  if (!turn) {
    throw std::exception();
  }

  auto visitor =
      Overload{[this](CardMove c) {
                 players[c.playerIdx].playCard(c.cardIdx, deck.pop_card());
                 return true;
               },
               [this](QueenMove q) {
                 if (q.type == QueenMove::KING) {
                   auto wokenQueen = wakeQueen(q.cardIdx);
                   if (!wokenQueen.has_value())
                     return false;
                   players[q.playerIdx].addQueen(*wokenQueen);
                 } else { // POTION/KNIGHT
                   auto queen = players[q.targetIdx].removeQueen(q.cardIdx);
                   if (q.type == QueenMove::KNIGHT)
                     players[q.playerIdx].addQueen(queen);
                   else // QueenMove::POTION
                     sleepyQueen(queen);
                 }
                 return true;
               }};

  for (auto &move : turn.moves) {
    io_handler.write_all(getMoveString(move));
    if (!std::visit(visitor, move)) {
      std::cout << "move visitor failed!" << std::endl;
      return false;
    }
  }
  return true;
}

bool Game::anyPlayerHasQueens() {
  return std::any_of(players.begin(), players.end(),
                     [](const Player &p) { return p.getQueenCount() > 0; });
}

std::optional<Queen> Game::peekQueen(int idx) {
  if (queens[idx].second == false)
    return {};
  return queens[idx].first;
}

std::optional<Queen> Game::wakeQueen(int idx) {
  auto queen = peekQueen(idx);
  queens[idx].second = false;
  return queen;
}

void Game::sleepyQueen(Queen q) {
  // auto it = queens.begin();
  auto it = std::find_if(queens.begin(), queens.end(),
                         [](auto &q) { return !q.second; });
  if (it == queens.end())
    throw std::exception();

  *it = std::make_pair(std::move(q), true);
}
