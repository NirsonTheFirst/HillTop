#include "commands.h"

#include <fstream>
#include <sstream>

void Commands::load(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) return;

  std::stringstream buf;
  buf << file.rdbuf();
  std::string text = buf.str();

  // Список всех ключей из text.json
  const char* keys[] = {
      "welcome",         "help",          "help_look",     "help_go",
      "help_take",       "help_inv",      "help_use",      "help_help",
      "help_quit",       "help_talk",     "help_duel",     "exits",
      "items_here",      "npcs_here",     "cant_go",       "no_location",
      "unknown",         "goodbye",       "game_over",     "cmd_help",
      "cmd_look",        "cmd_quit",      "cmd_go",        "cmd_take",
      "cmd_inv",         "cmd_use",       "cmd_talk",      "cmd_duel",
      "taken",           "not_found",     "npc_not_found", "inventory",
      "inventory_empty", "used",          "cant_use",      "duel_start",
      "duel_prompt",     "duel_word",     "duel_win",      "duel_lose",
      "duel_no_weapon",  "duel_no_enemy", "duel_slow",     "ending_win",
      "ending_lose",     "ending_dead"};

  for (const char* key : keys) {
    std::string search = "\"" + std::string(key) + "\"";
    size_t p = text.find(search);
    if (p != std::string::npos) {
      size_t c = text.find(':', p);
      if (c != std::string::npos) {
        size_t q1 = text.find('"', c + 1);
        if (q1 != std::string::npos) {
          size_t q2 = text.find('"', q1 + 1);
          if (q2 != std::string::npos) {
            std::string value = text.substr(q1 + 1, q2 - q1 - 1);
            textsCache[key] = value;
            if (std::string(key).rfind("cmd_", 0) == 0) {
              cmdMap[key] = value;
            }
          }
        }
      }
    }
  }
}

bool Commands::is(const std::string& input, const std::string& cmd) const {
  std::map<std::string, std::string>::const_iterator it = cmdMap.find(cmd);
  return it != cmdMap.end() && input == it->second;
}

std::string Commands::arg(const std::string& input,
                          const std::string& cmd) const {
  std::map<std::string, std::string>::const_iterator it = cmdMap.find(cmd);
  if (it == cmdMap.end()) return "";
  const std::string& c = it->second;
  return (input.find(c) == 0) ? input.substr(c.size()) : "";
}

std::string Commands::t(const std::string& key) const {
  std::map<std::string, std::string>::const_iterator it = textsCache.find(key);
  return (it != textsCache.end()) ? it->second : key;
}
