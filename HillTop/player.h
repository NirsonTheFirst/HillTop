#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

struct Item {
  std::string id, name, description, type;
  int damage, ammo, value;
  std::string flag;
  std::vector<std::string> aliases;
};

struct Player {
  std::string name;
  std::vector<Item> inventory;
};

#endif
