#ifndef GAME_H
#define GAME_H


#include <windows.h>
#include <mmsystem.h>

#include <map>
#include <string>

#include "commands.h"
#include "dialogue.h"
#include "duel.h"
#include "inventory.h"
#include "types.h"
#include "world.h"
#ifdef _WIN32
#pragma comment(lib, "winmm.lib")
#endif

class Game {
 public:
  Game();
  void init();
  void run();
  void shutdown();

 private:
  bool is_running = false;
  Player player;
  World world;
  Commands cmd;
  std::map<std::string, Item> items;
  std::map<std::string, NPC> npcs;
  std::map<std::string, Dialogue> dialogues;
  std::map<std::string, std::string> endings;
  std::map<std::string, bool> flags;

  Inventory inventory{items, player, cmd};
  DialogueManager dialogueMgr{npcs, dialogues, cmd};
  Duel duel{player, npcs, cmd};

  void showLocation();
  void moveTo(const std::string& target);
  void checkEndings();
  void showHelp();

  void pauseMusic();
  void resumeMusic();

  void playMusic();
  std::string currentMusic;
  bool musicPaused = false;
};

#endif
