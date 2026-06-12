#include "game.h"

#include <iostream>

void Game::showLocation() {
    Location& loc = world.here();

    std::cout << "--- " << loc.name << " ---" << std::endl;
    std::cout << loc.description << std::endl;

    if (!loc.npcs.empty()) {
        std::cout << cmd.t("npcs_here") << " ";

        for (size_t i = 0; i < loc.npcs.size(); i++) {
            std::string name = loc.npcs[i];

            if (npcs.count(name))
                name = npcs[name].name;

            std::cout << name;

            if (i < loc.npcs.size() - 1)
                std::cout << ", ";
        }

        std::cout << std::endl;
    }

    if (!loc.items.empty()) {
        std::cout << cmd.t("items_here") << " ";

        for (size_t i = 0; i < loc.items.size(); i++) {
            std::string name = loc.items[i];

            if (items.count(name))
                name = items[name].name;

            std::cout << name;

            if (i < loc.items.size() - 1)
                std::cout << ", ";
        }

        std::cout << std::endl;
    }

    std::cout << cmd.t("exits") << " ";

    for (size_t i = 0; i < loc.exits.size(); i++) {
        std::string name = loc.exits[i];

        if (world.all().count(name))
            name = world.all().at(name).name;

        std::cout << name;

        if (i < loc.exits.size() - 1)
            std::cout << ", ";
    }

    std::cout << std::endl << std::endl;
}

std::string Game::resolve(const std::string& input) {
    if (world.all().count(input))
        return input;

    for (std::map<std::string, Location>::const_iterator it = world.all().begin();
        it != world.all().end(); ++it) {

        if (it->second.name == input)
            return it->first;

        for (size_t i = 0; i < it->second.aliases.size(); i++)
            if (it->second.aliases[i] == input)
                return it->first;
    }

    return input;
}

void Game::moveTo(const std::string& target) {
    std::string id = resolve(target);

    if (!world.canGo(id)) {
        std::cout << cmd.t("cant_go") << std::endl;
        return;
    }

    world.goTo(id);
    showLocation();
}