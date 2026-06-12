#include "game.h"

#include <iostream>

Game::Game() : is_running(false) {}

void Game::init() {
    cmd.load("data/text.json");
    world.load("data/locations.json");
    items = Loader::loadItems("data/items.json");
    npcs = Loader::loadNPCs("data/npcs.json");
    dialogues = Loader::loadDialogues("data/dialogues.json");
    player = Loader::loadPlayer("data/player.json");

    std::cout << "=== HILLTOP ===" << std::endl;
    std::cout << cmd.t("welcome") << std::endl << std::endl;

    is_running = true;
    showLocation();
}

void Game::shutdown() {
    std::cout << cmd.t("game_over") << std::endl;
}