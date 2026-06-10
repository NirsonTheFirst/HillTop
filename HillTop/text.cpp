#include "text.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

std::map<std::string, std::string> Text::strings;

static std::string readFile(const std::string& path) {
  std::ifstream f(path);
  if (!f.is_open()) return "";
  std::stringstream b;
  b << f.rdbuf();
  return b.str();
}

static std::string extract(const std::string& t, const std::string& k) {
  std::string s = "\"" + k + "\"";
  size_t p = t.find(s);
  if (p == std::string::npos) return "";
  size_t c = t.find(':', p);
  size_t q1 = t.find('"', c + 1);
  size_t q2 = t.find('"', q1 + 1);
  return t.substr(q1 + 1, q2 - q1 - 1);
}

void Text::load(const std::string& path) {
  std::string t = readFile(path);
  if (t.empty()) return;

  const char* keys[] = {"welcome",     "help",      "help_look", "help_go",
                        "help_help",   "help_quit", "exits",     "cant_go",
                        "no_location", "unknown",   "goodbye",   "game_over",
                        "cmd_help",    "cmd_look",  "cmd_quit",  "cmd_go"};
  int count = 16;

  for (int i = 0; i < count; i++) {
    strings[keys[i]] = extract(t, keys[i]);
  }
}

std::string Text::get(const std::string& key) {
  if (strings.find(key) != strings.end()) {
    return strings[key];
  }
  return key;
}

bool Text::isCommand(const std::string& input, const std::string& cmdKey) {
  std::string cmd = get(cmdKey);
  if (cmd.empty()) return false;
  return input == cmd;
}

std::string Text::extractArg(const std::string& input,
                             const std::string& cmdKey) {
  std::string cmd = get(cmdKey);
  if (cmd.empty()) return "";
  if (input.find(cmd) == 0) {
    return input.substr(cmd.size());
  }
  return "";
}
