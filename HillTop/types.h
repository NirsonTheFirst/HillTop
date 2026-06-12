#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

struct Item {
  std::string id, name, description, type;
  int damage = 0, ammo = 0, value = 0;
  std::string flag;
  std::vector<std::string> aliases;
};

struct DialogueOption {
  std::string text, next, flag;
};

struct Dialogue {
  std::string text;
  std::vector<DialogueOption> options;
};

struct NPC {
  std::string id, name, description, dialogue;
  std::vector<std::string> aliases;
};

struct Location {
  std::string id, name, description;
  std::vector<std::string> exits, aliases, items, npcs;
};

struct Player {
  std::string name;
  int hp = 100, maxHp = 100;
  std::vector<Item> inventory;
};

#endif
