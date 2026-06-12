#ifndef GAME_H
#define GAME_H

#include <map>
#include <string>

#include "commands.h"
#include "player.h"
#include "world.h"

class Game {
 public:
  Game();
  void init();
  void run();
  void shutdown();

 private:
  bool is_running;
  Player player;
  World world;
  Commands cmd;
  std::map<std::string, Item> items;
  std::map<std::string, NPC> npcs;
  std::map<std::string, Dialogue> dialogues;
  std::map<std::string, bool> flags;

  void showLocation();
  void moveTo(const std::string& target);
  void takeItem(const std::string& name);
  void showInventory();
  void useItem(const std::string& name);
  void talkTo(const std::string& name);
  void runDialogue(const std::string& dialogueId);
  void duel();
  void showHelp();
  void checkEndings();

  Item* findItem(const std::string& name);
  NPC* findNPC(const std::string& name);
  std::string resolve(const std::string& input);
};

#endif
