#include "commands.h"

#include <fstream>
#include <sstream>

void Commands::load(const std::string& path) {
  std::ifstream file(path);
  std::stringstream buf;
  buf << file.rdbuf();
  std::string text = buf.str();

  std::string keys[] = {
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
      "ending_lose",     "ending_check"};
  for (auto& key : keys) {
    size_t p = text.find("\"" + key + "\"");
    if (p == std::string::npos) continue;
    size_t c = text.find(':', p);
    size_t q1 = text.find('"', c + 1);
    size_t q2 = text.find('"', q1 + 1);
    texts[key] = text.substr(q1 + 1, q2 - q1 - 1);
  }
}

bool Commands::is(const std::string& input, const std::string& cmd) const {
  return texts.count(cmd) && input == texts.at(cmd);
}

std::string Commands::arg(const std::string& input,
                          const std::string& cmd) const {
  if (!texts.count(cmd)) return "";
  const std::string& c = texts.at(cmd);
  return (input.find(c) == 0) ? input.substr(c.size()) : "";
}

std::string Commands::t(const std::string& key) const {
  return texts.count(key) ? texts.at(key) : key;
}
