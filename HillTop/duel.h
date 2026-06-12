#ifndef DUEL_H
#define DUEL_H

#include <map>
#include <string>
#include <vector>

#include "types.h"

class Commands;

class Duel {
 public:
  Duel(Player& player, std::map<std::string, NPC>& npcs, const Commands& cmd);

  bool fight(std::vector<std::string>& locNPCs,
             std::map<std::string, bool>& flags, bool& isRunning);

 private:
  Player& player;
  std::map<std::string, NPC>& npcs;
  const Commands& cmd;
};

#endif
