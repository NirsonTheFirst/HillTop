#include "game.h"

#include <fstream>
#include <iostream>
#include <sstream>

Game::Game() { is_running = false; }

void Game::loadTexts(const std::string& path) {
  std::ifstream file(path);
  std::stringstream buf;
  buf << file.rdbuf();
  std::string text = buf.str();

  std::string keys[] = {"welcome",     "help",      "help_look", "help_go",
                        "help_help",   "help_quit", "exits",     "cant_go",
                        "no_location", "unknown",   "goodbye",   "game_over",
                        "cmd_help",    "cmd_look",  "cmd_quit",  "cmd_go"};
  for (auto& key : keys) {
    size_t p = text.find("\"" + key + "\"");
    if (p == std::string::npos) continue;
    size_t c = text.find(':', p);
    size_t q1 = text.find('"', c + 1);
    size_t q2 = text.find('"', q1 + 1);
    texts[key] = text.substr(q1 + 1, q2 - q1 - 1);
  }
}

std::string Game::t(const std::string& key) {
  return texts.count(key) ? texts[key] : key;
}

bool Game::isCmd(const std::string& in, const std::string& key) {
  return texts.count(key) && in == texts[key];
}

std::string Game::arg(const std::string& in, const std::string& key) {
  if (!texts.count(key)) return "";
  std::string cmd = texts[key];
  return (in.find(cmd) == 0) ? in.substr(cmd.size()) : "";
}

void Game::loadLocations(const std::string& path) {
  std::ifstream file(path);
  std::stringstream buf;
  buf << file.rdbuf();
  std::string text = buf.str();

  int depth = 0;
  std::string id, obj;

  for (size_t i = 0; i < text.size(); i++) {
    char c = text[i];

    if (c == '{') {
      if (depth == 1 && !id.empty() && obj.empty()) obj = "{";
      depth++;
      if (!id.empty() && !obj.empty()) obj += c;
    } else if (c == '}') {
      depth--;
      if (!id.empty() && !obj.empty()) obj += c;
      if (depth == 1 && !id.empty() && !obj.empty()) {
        Location loc;
        loc.id = id;
        loc.name = getVal(obj, "name");
        loc.description = getVal(obj, "description");
        loc.exits = getArr(obj, "exits");
        loc.aliases = getArr(obj, "aliases");
        locations[id] = loc;
        id = "";
        obj = "";
      }
    } else if (depth == 1 && c == '"' && id.empty()) {
      size_t q2 = text.find('"', i + 1);
      id = text.substr(i + 1, q2 - i - 1);
      i = q2;
    } else if (!id.empty() && !obj.empty()) {
      obj += c;
    }
  }
}

std::string Game::getVal(const std::string& obj, const std::string& key) {
  size_t p = obj.find("\"" + key + "\"");
  if (p == std::string::npos) return "";
  size_t c = obj.find(':', p);
  size_t q1 = obj.find('"', c + 1);
  size_t q2 = obj.find('"', q1 + 1);
  return obj.substr(q1 + 1, q2 - q1 - 1);
}

std::vector<std::string> Game::getArr(const std::string& obj,
                                      const std::string& key) {
  std::vector<std::string> res;
  size_t p = obj.find("\"" + key + "\"");
  if (p == std::string::npos) return res;
  size_t b = obj.find('[', p);
  size_t e = obj.find(']', b);
  std::string s = obj.substr(b + 1, e - b - 1);
  size_t q = 0;
  while (true) {
    size_t q1 = s.find('"', q);
    if (q1 == std::string::npos) break;
    size_t q2 = s.find('"', q1 + 1);
    res.push_back(s.substr(q1 + 1, q2 - q1 - 1));
    q = q2 + 1;
  }
  return res;
}

void Game::init() {
  loadTexts("data/text.json");
  loadLocations("data/locations.json");

  std::cout << "=== HILLTOP ===" << std::endl;
  std::cout << t("welcome") << std::endl << std::endl;

  current_location_id = "bank";
  player.name = "Clerk";
  is_running = true;
  showLocation();
}

void Game::showLocation() {
  Location& loc = locations[current_location_id];
  std::cout << "--- " << loc.name << " ---" << std::endl;
  std::cout << loc.description << std::endl;
  std::cout << t("exits") << " ";

  for (size_t i = 0; i < loc.exits.size(); i++) {
    std::string name = loc.exits[i];
    if (locations.count(name)) name = locations[name].name;
    std::cout << name;
    if (i < loc.exits.size() - 1) std::cout << ", ";
  }
  std::cout << std::endl << std::endl;
}

void Game::moveTo(const std::string& input) {
  std::string target = input;

  for (auto& pair : locations) {
    if (pair.second.name == input) {
      target = pair.first;
      break;
    }
    for (auto& alias : pair.second.aliases) {
      if (alias == input) {
        target = pair.first;
        break;
      }
    }
  }

  Location& cur = locations[current_location_id];
  bool ok = false;
  for (auto& exit : cur.exits) {
    if (exit == target) {
      ok = true;
      break;
    }
  }

  if (!ok) {
    std::cout << t("cant_go") << std::endl;
    return;
  }

  current_location_id = target;
  showLocation();
}

void Game::processCommand(const std::string& input) {
  if (isCmd(input, "cmd_help")) {
    std::cout << t("help") << std::endl;
    std::cout << t("help_look") << std::endl;
    std::cout << t("help_go") << std::endl;
    std::cout << t("help_help") << std::endl;
    std::cout << t("help_quit") << std::endl;
    return;
  }
  if (isCmd(input, "cmd_look")) {
    showLocation();
    return;
  }
  if (isCmd(input, "cmd_quit")) {
    is_running = false;
    std::cout << t("goodbye") << std::endl;
    return;
  }

  std::string a = arg(input, "cmd_go");
  if (!a.empty()) {
    moveTo(a);
    return;
  }

  std::cout << t("unknown") << std::endl;
}

void Game::run() {
  std::string input;
  while (is_running) {
    std::cout << "> ";
    std::getline(std::cin, input);
    if (!input.empty()) processCommand(input);
  }
}

void Game::shutdown() { std::cout << t("game_over") << std::endl; }

bool Game::isRunning() const { return is_running; }
