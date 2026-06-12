#include "game.h"

#include <algorithm>
#include <iostream>

Item* Game::findItem(const std::string& name) {
    for (std::map<std::string, Item>::iterator it = items.begin();
        it != items.end(); ++it) {

        if (it->second.name == name)
            return &it->second;

        for (size_t i = 0; i < it->second.aliases.size(); i++)
            if (it->second.aliases[i] == name)
                return &it->second;
    }

    return NULL;
}

void Game::takeItem(const std::string& name) {
    Location& loc = world.here();

    for (size_t i = 0; i < loc.items.size(); i++) {
        std::string id = loc.items[i];

        if (!items.count(id))
            continue;

        Item& item = items[id];

        if (item.name == name ||
            std::find(item.aliases.begin(), item.aliases.end(), name) != item.aliases.end()) {

            player.inventory.push_back(item);

            std::cout << cmd.t("taken") << item.name << std::endl;

            loc.items.erase(loc.items.begin() + i);

            return;
        }
    }

    std::cout << cmd.t("not_found") << std::endl;
}

void Game::showInventory() {
    if (player.inventory.empty()) {
        std::cout << cmd.t("inventory_empty") << std::endl;
        return;
    }

    std::cout << cmd.t("inventory") << std::endl;

    for (size_t i = 0; i < player.inventory.size(); i++) {
        std::cout << "  "
            << i + 1
            << ". "
            << player.inventory[i].name
            << " ("
            << player.inventory[i].description
            << ")"
            << std::endl;
    }
}

void Game::useItem(const std::string& name) {
    for (size_t i = 0; i < player.inventory.size(); i++) {
        Item& item = player.inventory[i];
        if (item.name == name || std::find(item.aliases.begin(), item.aliases.end(),
            name) != item.aliases.end()) {
            std::cout << cmd.t("used") << item.name << std::endl;
            if (item.type == "consumable") {
                player.hp += item.value;
                if (player.hp > 100) player.hp = 100;
                std::cout << "  +" << item.value << " HP (" << player.hp << "/100)"
                    << std::endl;
                player.inventory.erase(player.inventory.begin() + i);
                return;
            }
            if (item.type == "story") {
                flags[item.flag] = true;
                std::cout << "  " << item.description << std::endl;
                return;
            }
            if (item.type == "weapon") {
                std::cout << "  Damage: " << item.damage << ", Ammo: " << item.ammo
                    << std::endl;
                return;
            }
        }
    }
    std::cout << cmd.t("cant_use") << std::endl;
}