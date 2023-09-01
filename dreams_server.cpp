#include "Game.hpp"

int main() {
  Game g(3);
  int i = 0;
  while (true) {
    g.printPlayers();
    g.PlayTurn(i);
    i = (i + 1) % 3;
  }
  g.printPlayers();

  return 0;
}