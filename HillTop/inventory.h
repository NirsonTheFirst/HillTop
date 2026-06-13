#ifndef INVENTORY_H
#define INVENTORY_H

#include <map>
#include <string>
#include <vector>

#include "types.h"

class Commands;

class Inventory {
 public:
  Inventory(std::map<std::string, Item>& items, Player& player,
            const Commands& cmd);

  void take(const std::string& name, std::vector<std::string>& locItems);
  void show() const;
  void use(const std::string& name, std::map<std::string, bool>& flags);

 private:
  std::map<std::string, Item>& items;
  Player& player;
  const Commands& cmd;
};

#endif
