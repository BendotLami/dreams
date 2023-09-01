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

template <typename... Ts> struct Overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;

class Queen {
public:
  enum Type {
    Cat = 0,
    Dog,
    Roses,
    Regular,
  };

  Queen(int points, Type type) : points(points), type(type) {}

  std::string toString() const {
    switch (type) {
    case Type::Cat:
      return "Cat";
    case Type::Dog:
      return "Dog";
    case Type::Regular:
      return "Regular";
    case Type::Roses:
      return "Roses";
    default:
      return "???";
    }
  }

private:
  int points;
  Type type;
};

class QueenFactory {
public:
  static std::vector<Queen> getQueens() {
    std::vector<Queen> q;
    q.emplace_back(15, Queen::Type::Cat);
    q.emplace_back(15, Queen::Type::Dog);
    q.emplace_back(5, Queen::Type::Roses);
    for (int i = 0; i < 9; i++)
      q.emplace_back(10, Queen::Type::Regular);
    return q;
  }
};

class King {};
class Dragon {};
class Wand {};
class Knight {};
class Potion {};

class Number {
public:
  Number(int value) : value(value){};
  int getValue() { return value; };

private:
  int value;
};

using Card = std::variant<King, Dragon, Wand, Knight, Potion, Number>;

template <typename T>
static auto isType = [](const Card &c) { return std::holds_alternative<T>(c); };

auto printCardVisitor =
    Overload{[](King c) { return 'K'; },
             [](Dragon c) { return 'd'; },
             [](Wand c) { return 'w'; },
             [](Knight c) { return 'k'; },
             [](Potion c) { return 'p'; },
             [](Number c) -> char {
               return c.getValue() == 10 ? 'A' : '0' + c.getValue();
             },
             [](auto c) { return "?"; }

    };

auto printCard = [](const Card &c) { return std::visit(printCardVisitor, c); };

class Player {
  template <typename T> bool hasType() {
    return std::find_if(cards.begin(), cards.end(), [](const Card &c) {
             return std::holds_alternative<T>(c);
           }) != cards.end();
  }

public:
  Player(std::list<Card> &&cards) : cards(cards){};
  //   Player(std::list<Card> cards) : cards(cards){};

  const std::list<Card> &getCards() { return cards; }

  std::string printQueens() const {
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

  int getQueenCount() const { return queens.size(); }

  std::string printCards() const {

    std::string s;
    s.append("Cards: ");
    auto it = cards.begin();
    for (int i = 0; i < cards.size(); i++) {
      s.append(std::to_string(i));
      s.append(": ");
      s.push_back(printCard(*it));
      s.push_back(' ');
      ++it;
    }
    s.pop_back();
    return s;
  }

  std::string toString() const {
    std::string s;
    s.append(printCards());
    s.push_back('\n');
    s.append(printQueens());
    s.push_back('\n');
    return s;
  }

  void addQueen(Queen q) { this->queens.emplace_back(std::move(q)); }

  Queen removeQueen(int idx) {
    auto it = queens.begin();
    std::advance(it, idx);
    Queen tmp = *it;
    queens.erase(it);
    return tmp;
  }

  Card peekCard(int idx) {
    auto it = cards.begin();
    std::advance(it, idx);
    return *it;
  }

  // For now, multiple cards is multiple calls
  Card playCard(int idx, Card nextCard) {
    auto it = cards.begin();
    std::advance(it, idx);
    std::swap(nextCard, *it);
    return nextCard;
  }

  bool hasPotion() { return hasType<Potion>(); }
  bool hasWand() { return hasType<Wand>(); }
  bool hasKnight() { return hasType<Knight>(); }
  bool hasDragon() { return hasType<Dragon>(); }
  bool hasKing() { return hasType<King>(); }

  friend std::ostream &operator<<(std::ostream &os, const Player &p) {
    os << p.toString();
    return os;
  }

private:
  std::list<Queen> queens;
  std::list<Card> cards;
};

class Deck {
  constexpr static int NumOfKings = 8;
  constexpr static int NumOfDragons = 8;
  constexpr static int NumOfWands = 8;
  constexpr static int NumOfKnights = 8;
  constexpr static int NumOfPotions = 8;
  constexpr static int NumOfNumberCards = 40;
  constexpr static int TotalCards = NumOfKings + NumOfDragons + NumOfWands +
                                    NumOfKnights + NumOfPotions +
                                    NumOfNumberCards;

public:
  Deck() {
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

  std::string toString() const {
    std::string s;
    for (const auto &c : cards) {
      s.push_back(std::visit(printCardVisitor, c));
      s.push_back(' ');
    }
    s.pop_back();
    return s;
  }

  friend std::ostream &operator<<(std::ostream &os, const Deck &d) {
    os << d.toString();
    return os;
  }

  Card pop_card() {
    if (idx >= nextShuffle) {
      shuffle();
    }
    Card temp = cards.front();
    cards.pop_front();
    idx++;
    return temp;
  };

  std::list<Card> pop_cards(int count) {
    std::list<Card> rtn;
    for (int i = 0; i < count; i++)
      rtn.push_back(pop_card());
    return rtn;
  }

  void push_card(Card c) { cards.push_back(c); }

private:
  void shuffle() {
    std::vector<Card> shuf(cards.size());
    std::copy(cards.begin(), cards.end(), shuf.begin());

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuf.begin(), shuf.end(), g);

    std::copy(shuf.begin(), shuf.end(), cards.begin());

    nextShuffle = cards.size();
    idx = 0;
  };

  std::list<Card> cards;
  int idx, nextShuffle;
};

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