#include "game.h"

#include <algorithm>
#include <iostream>

Game::Game() { is_running = false; }

void Game::init() {
  cmd.load("data/text.json");
  world.load("data/locations.json");
  items = Loader::loadItems("data/items.json");

  std::cout << "=== HILLTOP ===" << std::endl;
  std::cout << cmd.t("welcome") << std::endl << std::endl;
  is_running = true;
  showLocation();
}

void Game::showLocation() {
  Location& loc = world.here();
  std::cout << "--- " << loc.name << " ---" << std::endl;
  std::cout << loc.description << std::endl;

  if (!loc.items.empty()) {
    std::cout << cmd.t("items_here") << " ";
    for (size_t i = 0; i < loc.items.size(); i++) {
      std::string name = loc.items[i];
      if (items.count(name)) name = items[name].name;
      std::cout << name;
      if (i < loc.items.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
  }

  std::cout << cmd.t("exits") << " ";
  for (size_t i = 0; i < loc.exits.size(); i++) {
    std::string name = loc.exits[i];
    if (world.all().count(name)) name = world.all().at(name).name;
    std::cout << name;
    if (i < loc.exits.size() - 1) std::cout << ", ";
  }
  std::cout << std::endl << std::endl;
}

std::string Game::resolve(const std::string& input) {
  if (world.all().count(input)) return input;
  for (auto& pair : world.all()) {
    if (pair.second.name == input) return pair.first;
    for (auto& alias : pair.second.aliases)
      if (alias == input) return pair.first;
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

Item* Game::findItem(const std::string& name) {
  for (auto& pair : items) {
    if (pair.second.name == name) return &pair.second;
    for (auto& alias : pair.second.aliases)
      if (alias == name) return &pair.second;
  }
  return nullptr;
}

void Game::takeItem(const std::string& name) {
  Location& loc = world.here();
  for (size_t i = 0; i < loc.items.size(); i++) {
    std::string id = loc.items[i];
    if (!items.count(id)) continue;
    Item& item = items[id];
    if (item.name == name || std::find(item.aliases.begin(), item.aliases.end(),
                                       name) != item.aliases.end()) {
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
  for (size_t i = 0; i < player.inventory.size(); i++)
    std::cout << "  " << (i + 1) << ". " << player.inventory[i].name << " ("
              << player.inventory[i].description << ")" << std::endl;
}

void Game::useItem(const std::string& name) {
  for (size_t i = 0; i < player.inventory.size(); i++) {
    Item& item = player.inventory[i];
    if (item.name == name || std::find(item.aliases.begin(), item.aliases.end(),
                                       name) != item.aliases.end()) {
      std::cout << cmd.t("used") << item.name << std::endl;
      if (item.type == "consumable") {
        std::cout << "  +" << item.value << " HP" << std::endl;
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

void Game::showHelp() {
  std::cout << cmd.t("help") << std::endl;
  std::cout << cmd.t("help_look") << std::endl;
  std::cout << cmd.t("help_go") << std::endl;
  std::cout << cmd.t("help_take") << std::endl;
  std::cout << cmd.t("help_inv") << std::endl;
  std::cout << cmd.t("help_use") << std::endl;
  std::cout << cmd.t("help_help") << std::endl;
  std::cout << cmd.t("help_quit") << std::endl;
}

void Game::run() {
  std::string input;
  while (is_running) {
    std::cout << "> ";
    std::getline(std::cin, input);
    if (input.empty()) continue;

    if (cmd.is(input, "cmd_help")) {
      showHelp();
      continue;
    }
    if (cmd.is(input, "cmd_look")) {
      showLocation();
      continue;
    }
    if (cmd.is(input, "cmd_quit")) {
      is_running = false;
      std::cout << cmd.t("goodbye") << std::endl;
      continue;
    }
    if (cmd.is(input, "cmd_inv")) {
      showInventory();
      continue;
    }

    std::string a = cmd.arg(input, "cmd_go");
    if (!a.empty()) {
      moveTo(a);
      continue;
    }
    a = cmd.arg(input, "cmd_take");
    if (!a.empty()) {
      takeItem(a);
      continue;
    }
    a = cmd.arg(input, "cmd_use");
    if (!a.empty()) {
      useItem(a);
      continue;
    }

    std::cout << cmd.t("unknown") << std::endl;
  }
}

void Game::shutdown() { std::cout << cmd.t("game_over") << std::endl; }
