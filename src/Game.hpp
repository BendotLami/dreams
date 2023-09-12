#pragma once

#include "Deck.hpp"
#include "IOhandler.hpp"
#include "Player.hpp"
#include "Queen.hpp"
#include "Turn.hpp"

class Game {
  constexpr static int NumOfQueens = 12;
  constexpr static int CardsPerPlayer = 5;

  void initCards(int players);

  awaitable<char> readInput(int playerIdx, std::string msg,
                            std::function<bool(int)> predicate);

public:
  Game(int players, IOHandler &io_handler);

  // Game(int player) : Game(player, IOStreamHandler()){};

  std::string getPlayerHand(int player);

private:
  std::string playersString();

  void printPlayers();

  std::string queensString(bool debug);

  std::optional<int> handleDragon(int attackedIdx);

  std::optional<int> handleWand(int attackedIdx);

  awaitable<Turn> playKnight(int currentPlayer);

  awaitable<Turn> playKing(int currentPlayer);

  awaitable<Turn> playPotion(int currentPlayer);

public:
  awaitable<void> PlayTurn(int playerIdx);

private:
  bool commitTurn(const Turn &turn);
  bool anyPlayerHasQueens();

  std::optional<Queen> wakeQueen(int idx);

  std::optional<Queen> peekQueen(int idx);

  void sleepyQueen(Queen q);

  Deck deck;
  Players players;
  Queens queens;
  IOHandler &io_handler;
};
