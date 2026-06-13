#include "duel.h"
#include "commands.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

Duel::Duel(Player& p, std::map<std::string, NPC>& n, const Commands& c)
    : player(p), npcs(n), cmd(c) {
}

bool Duel::fight(std::vector<std::string>& locNPCs, std::map<std::string, bool>& flags, bool& isRunning) {
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
        if (item.type == "weapon") { hasWeapon = true; break; }

    if (!hasWeapon) {
        std::cout << cmd.t("duel_no_weapon") << std::endl;
        player.hp -= player.duelDamageNoWeapon;
        std::cout << "[" << player.hp << "/" << player.maxHp << " HP]" << std::endl;
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
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();

    if (input == targetWord && elapsed <= player.duelTimeLimitFast) {
        std::cout << cmd.t("duel_win") << std::endl;
        if (enemyId == "outlaw_leader") flags["leader_defeated"] = true;
        locNPCs.erase(std::remove(locNPCs.begin(), locNPCs.end(), enemyId), locNPCs.end());
        return true;
    }

    int damage = (input != targetWord)
        ? (elapsed > player.duelTimeLimitSlow ? player.duelDamageMisspellVerySlow :
            elapsed > player.duelTimeLimitFast ? player.duelDamageMisspellSlow : player.duelDamageMisspellFast)
        : (elapsed > player.duelTimeLimitSlow ? player.duelDamageVerySlow : player.duelDamageSlow);

    std::cout << cmd.t("duel_lose") << std::endl;
    std::cout << cmd.t("duel_damage") << damage << " HP" << std::endl;

    player.hp -= damage;
    if (player.hp <= 0) isRunning = false;
    std::cout << "[" << player.hp << "/" << player.maxHp << " HP]" << std::endl;

    return false;
}