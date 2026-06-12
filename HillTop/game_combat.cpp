#include "game.h"

#include <chrono>
#include <iostream>
#include <thread>

void Game::duel() {
    Location& loc = world.here();

    bool hasEnemy = false;

    for (size_t i = 0; i < loc.npcs.size(); i++) {
        std::string npcId = loc.npcs[i];

        if (npcs.count(npcId) &&
            npcs[npcId].dialogue.empty()) {

            hasEnemy = true;
            break;
        }
    }

    if (!hasEnemy) {
        std::cout << cmd.t("duel_no_enemy") << std::endl;
        return;
    }

    bool hasWeapon = false;

    for (size_t i = 0; i < player.inventory.size(); i++) {
        if (player.inventory[i].type == "weapon") {
            hasWeapon = true;
            break;
        }
    }

    if (!hasWeapon) {
        std::cout << cmd.t("duel_no_weapon") << std::endl;
        return;
    }

    std::string targetWord = cmd.t("duel_word");

    std::cout << cmd.t("duel_start") << std::endl;

    std::this_thread::sleep_for(
        std::chrono::milliseconds(500));

    for (int i = 3; i > 0; i--) {
        std::cout << i << "..." << std::endl;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(500));
    }

    std::cout
        << cmd.t("duel_prompt")
        << "'"
        << targetWord
        << "'"
        << std::endl;

    std::cout << "> ";

    auto start =
        std::chrono::steady_clock::now();

    std::string input;
    std::getline(std::cin, input);

    auto end =
        std::chrono::steady_clock::now();

    auto elapsed =
        std::chrono::duration_cast<
        std::chrono::milliseconds>(
            end - start).count();

    if (input == targetWord &&
        elapsed <= 3000) {

        std::cout
            << cmd.t("duel_win")
            << std::endl;

        flags["won_duel"] = true;
        flags["outlaw_defeated"] = true;
    }
    else if (input != targetWord) {

        std::cout
            << cmd.t("duel_lose")
            << std::endl;

        is_running = false;
    }
    else {

        std::cout
            << cmd.t("duel_slow")
            << std::endl;

        std::cout
            << cmd.t("duel_lose")
            << std::endl;

        is_running = false;
    }
}

void Game::killPlayer() {
    player.hp = 0;

    std::cout
        << cmd.t("ending_dead")
        << std::endl;

    is_running = false;
}

void Game::checkEndings() {

    if (flags["won_duel"] ||
        flags["outlaw_defeated"]) {

        std::cout
            << cmd.t("ending_win")
            << std::endl;

        is_running = false;
    }

    if (player.hp <= 0) {

        std::cout
            << cmd.t("ending_lose")
            << std::endl;

        is_running = false;
    }
}