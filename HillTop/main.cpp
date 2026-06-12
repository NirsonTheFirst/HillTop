#include <windows.h>

#include "game.h"

int main() {
  SetConsoleOutputCP(1251);
  SetConsoleCP(1251);
  Game game;
  game.init();
  game.run();
  game.shutdown();
  return 0;
}
