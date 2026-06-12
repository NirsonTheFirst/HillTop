#include "duel.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

#include "commands.h"

Duel::Duel(Player& p, std::map<std::string, NPC>& n, const Commands& c)
    : player(p), npcs(n), cmd(c) {}

bool Duel::fight(std::vector<std::string>& locNPCs,
                 std::map<std::string, bool>& flags, bool& isRunning) {
  std::string enemyId;
  for (auto& npcId : locNPCs) {
    auto it = npcs.find(npcId);
    if (it != npcs.end() && it->second.dialogue.empty()) {
      enemyId = npcId;
      break;
    }
  }

  if (enemyId.empty()) {
    std::cout << cmd.t("duel_no_enemy") << std::endl;
    return false;
  }

  bool hasWeapon = false;
  for (const auto& item : player.inventory)
    if (item.type == "weapon") {
      hasWeapon = true;
      break;
    }

  if (!hasWeapon) {
    std::cout << cmd.t("duel_no_weapon") << std::endl;
    return false;
  }

  const std::string targetWord = cmd.t("duel_word");

  std::cout << cmd.t("duel_start") << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  for (int i = 3; i > 0; i--) {
    std::cout << i << "..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  std::cout << cmd.t("duel_prompt") << "'" << targetWord << "'" << std::endl;
  std::cout << "> ";

  auto start = std::chrono::steady_clock::now();

  std::string input;
  std::getline(std::cin, input);

  auto end = std::chrono::steady_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();

  if (input == targetWord && elapsed <= 3000) {
    std::cout << cmd.t("duel_win") << std::endl;
    flags["won_duel"] = true;
    locNPCs.erase(std::remove(locNPCs.begin(), locNPCs.end(), enemyId),
                  locNPCs.end());
    return true;
  }

  std::cout << (input != targetWord ? cmd.t("duel_lose") : cmd.t("duel_slow"))
            << std::endl;
  player.hp = 0;
  isRunning = false;
  return false;
}
