#include "game.h"

#include <iostream>

#include "loader.h"

Game::Game() {}

void Game::init() {
  cmd.load("data/text.json");
  world.load("data/locations.json");
  items = Loader::loadItems("data/items.json");
  npcs = Loader::loadNPCs("data/npcs.json");
  dialogues = Loader::loadDialogues("data/dialogues.json");
  endings = Loader::loadStrings("data/endings.json");
  player = Loader::loadPlayer("data/player.json");

  std::cout << "=== HILLTOP ===" << std::endl;
  std::cout << cmd.t("welcome") << std::endl << std::endl;
  is_running = true;
  showLocation();
  playMusic();
}

void Game::showLocation() {
  const Location& loc = world.here();
  std::cout << "--- " << loc.name << " ---" << std::endl;
  std::cout << loc.description << std::endl;

  if (!loc.npcs.empty()) {
    std::cout << cmd.t("npcs_here") << " ";
    for (size_t i = 0; i < loc.npcs.size(); i++) {
      auto it = npcs.find(loc.npcs[i]);
      std::cout << (it != npcs.end() ? it->second.name : loc.npcs[i]);
      if (i < loc.npcs.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
  }

  if (!loc.items.empty()) {
    std::cout << cmd.t("items_here") << " ";
    for (size_t i = 0; i < loc.items.size(); i++) {
      auto it = items.find(loc.items[i]);
      std::cout << (it != items.end() ? it->second.name : loc.items[i]);
      if (i < loc.items.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
  }

  std::cout << cmd.t("exits") << " ";
  for (size_t i = 0; i < loc.exits.size(); i++) {
    auto it = world.all().find(loc.exits[i]);
    std::cout << (it != world.all().end() ? it->second.name : loc.exits[i]);
    if (i < loc.exits.size() - 1) std::cout << ", ";
  }
  std::cout << std::endl << std::endl;
}

void Game::moveTo(const std::string& target) {
  std::string id = world.resolve(target);
  if (!world.canGo(id)) {
    std::cout << cmd.t("cant_go") << std::endl;
    return;
  }
  world.goTo(id);
  playMusic();
  showLocation();
  for (auto& npcId : world.here().npcs) {
      auto it = npcs.find(npcId);
      if (it != npcs.end() && it->second.dialogue.empty()) {
          std::cout << it->second.name << cmd.t("enemy_attack") << std::endl;
          duel.fight(world.here().npcs, flags, is_running);
          checkEndings();
          break;
      }
  }
}

void Game::checkEndings() {
    if (player.hp <= 0) {
        std::cout << std::endl << endings["ending_dead"] << std::endl;
        is_running = false;
        return;
    }
    if (flags["leader_defeated"] && flags["joined_sheriff"] &&
        flags["heard_about_bandits"] && flags["know_about_mine"] &&
        flags["has_horseshoe"] && flags["has_letter"]) {
        std::cout << std::endl << endings["ending_good"] << std::endl;
        is_running = false;
        return;
    }
    if (flags["leader_defeated"] && flags["has_gold"]) {
        std::cout << std::endl << endings["ending_bad"] << std::endl;
        is_running = false;
        return;
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
  std::cout << cmd.t("help_pause") << std::endl;
  std::cout << cmd.t("help_resume") << std::endl;
  std::cout << cmd.t("help_help") << std::endl;
  std::cout << cmd.t("help_quit") << std::endl;
}

void Game::playMusic() {
#ifdef _WIN32
    std::string musicFile = world.here().music;
    if (musicFile.empty()) musicFile = "default";
    if (musicPaused) {
        if (currentMusic == musicFile) return;
        if (!currentMusic.empty()) {
            mciSendStringA(("close " + currentMusic).c_str(), NULL, 0, 0);
        }
        mciSendStringA(("open \"music/" + musicFile + ".mp3\" alias " + musicFile).c_str(), NULL, 0, 0);
        currentMusic = musicFile;
        return;
    }

    if (currentMusic == musicFile) return;

    if (!currentMusic.empty()) {
        mciSendStringA(("close " + currentMusic).c_str(), NULL, 0, 0);
    }
    mciSendStringA(("open \"music/" + musicFile + ".mp3\" alias " + musicFile).c_str(), NULL, 0, 0);
    mciSendStringA(("play " + musicFile + " repeat").c_str(), NULL, 0, 0);
    currentMusic = musicFile;
#endif
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
      inventory.show();
      continue;
    }
    if (cmd.is(input, "cmd_duel")) {
      duel.fight(world.here().npcs, flags, is_running);
      checkEndings();
      continue;
    }
    if (cmd.is(input, "cmd_pause")) {
        pauseMusic();
        continue;
    }
    if (cmd.is(input, "cmd_resume")) {
        resumeMusic();
        continue;
    }
    std::string a = cmd.arg(input, "cmd_go");
    if (!a.empty()) {
      moveTo(a);
      continue;
    }
    a = cmd.arg(input, "cmd_take");
    if (!a.empty()) {
      inventory.take(a, world.here().items);
      checkEndings();
      continue;
    }
    a = cmd.arg(input, "cmd_use");
    if (!a.empty()) {
      inventory.use(a, flags);
      checkEndings();
      continue;
    }
    a = cmd.arg(input, "cmd_talk");
    if (!a.empty()) {
      dialogueMgr.talk(a, world.here().npcs, flags);
      checkEndings();
      continue;
    }
    std::cout << cmd.t("unknown") << std::endl;
  }
}
void Game::pauseMusic() {
#ifdef _WIN32
    if (currentMusic.empty()) {
        std::cout << cmd.t("music_not_playing") << std::endl;
        return;
    }
    if (musicPaused) return;

    mciSendStringA(("pause " + currentMusic).c_str(), NULL, 0, NULL);
    musicPaused = true;
    std::cout << cmd.t("music_paused") << std::endl;
#endif
}

void Game::resumeMusic() {
#ifdef _WIN32
    if (currentMusic.empty()) {
        std::cout << cmd.t("music_not_playing") << std::endl;
        return;
    }
    if (!musicPaused) return;

    mciSendStringA(("resume " + currentMusic).c_str(), NULL, 0, NULL);
    musicPaused = false;
    std::cout << cmd.t("music_resumed") << std::endl;
#endif
}

void Game::shutdown() {
    mciSendStringA("close all", NULL, 0, 0);
    musicPaused = false;
    std::cout << cmd.t("game_over") << std::endl;
}
