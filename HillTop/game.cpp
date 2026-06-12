#include "game.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

Game::Game() { is_running = false; }

void Game::init() {
  cmd.load("data/text.json");
  world.load("data/locations.json");
  items = Loader::loadItems("data/items.json");
  npcs = Loader::loadNPCs("data/npcs.json");
  dialogues = Loader::loadDialogues("data/dialogues.json");

  std::cout << "=== HILLTOP ===" << std::endl;
  std::cout << cmd.t("welcome") << std::endl << std::endl;
  is_running = true;
  showLocation();
}

void Game::showLocation() {
  Location& loc = world.here();
  std::cout << "--- " << loc.name << " ---" << std::endl;
  std::cout << loc.description << std::endl;

  if (!loc.npcs.empty()) {
    std::cout << cmd.t("npcs_here") << " ";
    for (size_t i = 0; i < loc.npcs.size(); i++) {
      std::string name = loc.npcs[i];
      if (npcs.count(name)) name = npcs[name].name;
      std::cout << name;
      if (i < loc.npcs.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
  }

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

NPC* Game::findNPC(const std::string& name) {
  for (auto& pair : npcs) {
    if (pair.second.name == name) return &pair.second;
    for (auto& alias : pair.second.aliases)
      if (alias == name) return &pair.second;
  }
  return nullptr;
}

void Game::talkTo(const std::string& name) {
  Location& loc = world.here();
  for (auto& npcId : loc.npcs) {
    if (!npcs.count(npcId)) continue;
    NPC& npc = npcs[npcId];
    if (npc.name == name || std::find(npc.aliases.begin(), npc.aliases.end(),
                                      name) != npc.aliases.end()) {
      runDialogue(npc.dialogue);
      return;
    }
  }
  std::cout << cmd.t("npc_not_found") << std::endl;
}

void Game::runDialogue(const std::string& dialogueId) {
  if (!dialogues.count(dialogueId)) return;
  Dialogue& d = dialogues[dialogueId];

  std::cout << std::endl << d.text << std::endl << std::endl;

  if (d.options.empty()) return;

  std::vector<int> valid;
  for (size_t i = 0; i < d.options.size(); i++) {
    std::string req = d.options[i].flag;
    if (!req.empty() && !flags[req]) continue;
    valid.push_back((int)i);
    std::cout << "  " << valid.size() << ". " << d.options[i].text << std::endl;
  }
  std::cout << std::endl;

  if (valid.empty()) return;

  std::cout << "> ";
  std::string input;
  std::getline(std::cin, input);

  if (input.empty()) return;

  try {
    int choice = std::stoi(input) - 1;
    if (choice >= 0 && choice < (int)valid.size()) {
      DialogueOption& opt = d.options[valid[choice]];
      if (!opt.flag.empty()) flags[opt.flag] = true;
      if (!opt.next.empty()) runDialogue(opt.next);
    }
  } catch (...) {
    return;
  }
}

void Game::duel() {
  Location& loc = world.here();

  bool hasEnemy = false;
  for (auto& npcId : loc.npcs) {
    if (npcs.count(npcId) && npcs[npcId].dialogue.empty()) {
      hasEnemy = true;
      break;
    }
  }

  if (!hasEnemy) {
    std::cout << cmd.t("duel_no_enemy") << std::endl;
    return;
  }

  bool hasWeapon = false;
  for (auto& item : player.inventory) {
    if (item.type == "weapon") {
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
    flags["outlaw_defeated"] = true;
  } else if (input != targetWord) {
    std::cout << cmd.t("duel_lose") << std::endl;
    is_running = false;
  } else {
    std::cout << cmd.t("duel_slow") << std::endl;
    std::cout << cmd.t("duel_lose") << std::endl;
    is_running = false;
  }
}

void Game::checkEndings() {
  if (flags["outlaw_defeated"]) {
    std::cout << std::endl;
    std::cout << cmd.t("ending_win") << std::endl;
    is_running = false;
  }
}

void Game::showHelp() {
  std::cout << cmd.t("help") << std::endl;
  std::cout << cmd.t("help_look") << std::endl;
  std::cout << cmd.t("help_go") << std::endl;
  std::cout << cmd.t("help_take") << std::endl;
  std::cout << cmd.t("help_inv") << std::endl;
  std::cout << cmd.t("help_use") << std::endl;
  std::cout << cmd.t("help_talk") << std::endl;
  std::cout << cmd.t("help_duel") << std::endl;
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
    if (cmd.is(input, "cmd_duel")) {
      duel();
      checkEndings();
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
    a = cmd.arg(input, "cmd_talk");
    if (!a.empty()) {
      talkTo(a);
      continue;
    }

    std::cout << cmd.t("unknown") << std::endl;
  }
}

void Game::shutdown() { std::cout << cmd.t("game_over") << std::endl; }
