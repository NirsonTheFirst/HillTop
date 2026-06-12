#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

struct Item {
  std::string id, name, description, type;
  int damage = 0;
  int ammo = 0;
  int value = 0;
  std::string flag;
  std::vector<std::string> aliases;
};

struct DialogueOption {
  std::string text;
  std::string next;
  std::string flag;
};

struct Dialogue {
  std::string text;
  std::vector<DialogueOption> options;
};

struct NPC {
  std::string id, name, description, dialogue;
  std::vector<std::string> aliases;
};

struct Player {
  std::string name;
  std::vector<Item> inventory;
};

#endif
