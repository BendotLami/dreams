#pragma once

#include "Deck.hpp"
#include "IOhandler.hpp"
#include "Player.hpp"
#include "Queen.hpp"

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

  std::string queensString();

  bool handleDragon(int attackedIdx);

  bool handleWand(int attackedIdx);

  awaitable<bool> playKnight(int currentPlayer);

  awaitable<bool> playKing(int currentPlayer);

  awaitable<bool> playPotion(int currentPlayer);

public:
  awaitable<void> PlayTurn(int playerIdx);

private:
  bool anyPlayerHasQueens();

  std::optional<Queen> wakeQueen(int idx);

  void sleepyQueen(Queen q);

  Deck deck;
  std::vector<Player> players;
  std::vector<std::pair<Queen, bool>> queens;
  IOHandler &io_handler;
};
