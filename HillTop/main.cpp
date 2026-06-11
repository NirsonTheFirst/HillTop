#include <windows.h>

#include "game.h"

int main() {
  SetConsoleOutputCP(65001);
  SetConsoleCP(65001);
  Game game;
  game.init();
  game.run();
  game.shutdown();
  return 0;
}
