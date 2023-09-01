#include "Cards.hpp"
#include "Deck.hpp"
#include "Player.hpp"
#include "Queen.hpp"
#include "overload.hpp"
#include <algorithm>
#include <any>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <optional>
#include <random>
#include <ranges>
#include <sstream>
#include <variant>
#include <vector>

auto readInput = [](std::string msg, std::function<bool(int)> predicate) {
  auto validateInput = [&predicate](const std::string &input) {
    if (input.size() != 1)
      return false;
    unsigned char inputIdx = input[0] - '0';
    return predicate(inputIdx);
  };

  std::string input;
  do {
    std::cout << msg;
    std::cin >> input;
  } while (!validateInput(input));
  return input[0] - '0';
};

class Game {
  constexpr static int NumOfQueens = 12;
  constexpr static int CardsPerPlayer = 5;

  void initCards(int players) {
    for (int i = 0; i < players; i++) {
      this->players.push_back(deck.pop_cards(CardsPerPlayer));
    }
  }

public:
  Game(int players) {
    initCards(players);
    std::vector<Queen> tmp = QueenFactory::getQueens();
    std::transform(tmp.begin(), tmp.end(), std::back_inserter(queens),
                   [](Queen &q) { return std::pair<Queen, bool>(q, true); });
  };

  std::string playersString() {
    std::stringstream ss;
    for (int i = 0; i < players.size(); i++) {
      ss << "Player " << i << ":\n";
      ss << players[i] << "\n";
    }
    return ss.str();
  }

  void printPlayers() { std::cout << playersString(); }

  std::string queensString() {
    std::string msg;
    for_each(queens.begin(), queens.end(), [&msg](std::pair<Queen, bool> a) {
      msg.append(a.second ? a.first.toString() : "_");
      msg.push_back(' ');
    });
    msg.pop_back();

    return msg;
  }

  bool playKnight(int currentPlayer) {
    std::stringstream msg;

    if (!anyPlayerHasQueens()) {
      std::cout << "No player have any queens." << std::endl;
      return false;
    }

    bool hasQueens = false;
    int attackPlayerIdx;
    while (!hasQueens) {
      msg << "Players"
          << "\n";
      msg << playersString();
      msg << "Insert which player you want to attack: ";
      attackPlayerIdx = readInput(msg.str(), [this, currentPlayer](int i) {
        return i >= 0 && i < players.size() && i != currentPlayer;
      });
      hasQueens = players[attackPlayerIdx].getQueenCount() > 0;
      std::cout << "Player does not have queens." << std::endl;
    }

    msg.str(std::string());
    msg << players[attackPlayerIdx].printQueens() << '\n';
    msg << "Pick which queen you want to steal: ";
    int queenIdx = readInput(msg.str(), [this, attackPlayerIdx](int i) {
      return i >= 0 && i < players[attackPlayerIdx].getQueenCount();
    });

    players[currentPlayer].addQueen(
        players[attackPlayerIdx].removeQueen(queenIdx));

    return true;
  }

  bool playKing(int currentPlayer) {
    std::stringstream msg;

    std::vector<int> range;
    for (int i = 0; i < queens.size(); i++)
      if (queens[i].second)
        range.push_back(i);

    if (range.size() == 0) {
      std::cout << "No hidden queens available" << std::endl;
      return false;
    }

    auto isQueenIdxValid = [&range](int i) {
      for (const auto j : range)
        if (i == j)
          return true;
      return false;
    };

    msg << "Hidden Queens:\n";
    msg << queensString();
    msg << "\n"
        << "Insert chosen queen: ";

    int input = readInput(msg.str(), isQueenIdxValid);
    players[currentPlayer].addQueen(wakeQueen(input).value());

    return true;
  }

  void PlayTurn(int playerIdx) {
    std::stringstream msg;
    auto &player = players[playerIdx];
    msg << "Player " << playerIdx << "\n";
    msg << player;
    msg << "Insert which card you want to play: ";

    bool res = false;
    int cardIdx;

    while (!res) {
      int cardIdx = readInput(
          msg.str(), [this](int i) { return i >= 0 && i < CardsPerPlayer; });

      // Card playedCard = player.playCard(cardIdx, deck.pop_card());
      Card peekedCard = player.peekCard(cardIdx);
      std::cout << printCard(peekedCard) << std::endl;

      res = std::visit(
          Overload{
              [](Number n) { return true; },
              [this, playerIdx](Knight k) { return playKnight(playerIdx); },
              [this, playerIdx](King k) { return playKing(playerIdx); },
              [](auto c) {
                std::cout << "errer: " << printCard(c) << std::endl;
                return true;
              }},
          peekedCard);
    }

    // if (res)
    player.playCard(cardIdx, deck.pop_card());
  }

private:
  bool anyPlayerHasQueens() {
    return std::any_of(players.begin(), players.end(),
                       [](const Player &p) { return p.getQueenCount() > 0; });
  }

  std::optional<Queen> wakeQueen(int idx) {
    if (queens[idx].second == false)
      return {};
    queens[idx].second = false;
    return queens[idx].first;
  }

  void sleepyQueen(int idx) { queens[idx].second = true; }

  Deck deck;
  std::vector<Player> players;
  std::vector<std::pair<Queen, bool>> queens;
};

int main() {
  // asdf
  Game g(3);
  g.printPlayers();
  g.PlayTurn(0);
  g.printPlayers();

  return 0;
}