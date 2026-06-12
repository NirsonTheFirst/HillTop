#include "loader.h"

#include <fstream>
#include <sstream>

std::string Loader::getVal(const std::string& obj, const std::string& key) {
  size_t p = obj.find("\"" + key + "\"");
  if (p == std::string::npos) return "";
  size_t c = obj.find(':', p);
  size_t q1 = obj.find('"', c + 1);
  size_t q2 = obj.find('"', q1 + 1);
  return obj.substr(q1 + 1, q2 - q1 - 1);
}
Player Loader::loadPlayer(const std::string& path)
{
    Player player;

    std::ifstream file(path);
    std::stringstream buffer;

    buffer << file.rdbuf();

    std::string text = buffer.str();

    player.name = getVal(text, "name");
    player.hp = getInt(text, "hp");
    player.karma = getInt(text, "karma");

    return player;
}

std::vector<std::string> Loader::getArr(const std::string& obj,
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

int Loader::getInt(const std::string& obj, const std::string& key) {
  size_t p = obj.find("\"" + key + "\"");
  if (p == std::string::npos) return 0;
  size_t c = obj.find(':', p);
  size_t start = c + 1;
  while (start < obj.size() && (obj[start] == ' ' || obj[start] == '\n'))
    start++;
  size_t end = start;
  while (end < obj.size() && obj[end] >= '0' && obj[end] <= '9') end++;
  return std::stoi(obj.substr(start, end - start));
}

std::map<std::string, std::string> Loader::loadTexts(const std::string& path) {
  std::map<std::string, std::string> texts;
  std::ifstream file(path);
  std::stringstream buf;
  buf << file.rdbuf();
  std::string text = buf.str();

  std::string keys[] = {
      "welcome",       "help",      "help_look",       "help_go",
      "help_take",     "help_inv",  "help_use",        "help_help",
      "help_quit",     "help_talk", "exits",           "items_here",
      "npcs_here",     "cant_go",   "no_location",     "unknown",
      "goodbye",       "game_over", "cmd_help",        "cmd_look",
      "cmd_quit",      "cmd_go",    "cmd_take",        "cmd_inv",
      "cmd_use",       "cmd_talk",  "taken",           "not_found",
      "npc_not_found", "inventory", "inventory_empty", "used",
      "cant_use"};
  for (auto& key : keys) {
    size_t p = text.find("\"" + key + "\"");
    if (p == std::string::npos) continue;
    size_t c = text.find(':', p);
    size_t q1 = text.find('"', c + 1);
    size_t q2 = text.find('"', q1 + 1);
    texts[key] = text.substr(q1 + 1, q2 - q1 - 1);
  }
  return texts;
}

std::map<std::string, Location> Loader::loadLocations(const std::string& path) {
  std::map<std::string, Location> locations;
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
        loc.items = getArr(obj, "items");
        loc.npcs = getArr(obj, "npcs");
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
  return locations;
}

std::map<std::string, NPC> Loader::loadNPCs(const std::string& path) {
  std::map<std::string, NPC> npcs;
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
        NPC npc;
        npc.id = id;
        npc.name = getVal(obj, "name");
        npc.description = getVal(obj, "description");
        npc.dialogue = getVal(obj, "dialogue_id");
        npc.aliases = getArr(obj, "aliases");
        npcs[id] = npc;
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
  return npcs;
}

std::map<std::string, Item> Loader::loadItems(const std::string& path) {
  std::map<std::string, Item> items;
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
        Item item;
        item.id = id;
        item.name = getVal(obj, "name");
        item.description = getVal(obj, "description");
        item.type = getVal(obj, "type");
        item.damage = getInt(obj, "damage");
        item.ammo = getInt(obj, "ammo");
        item.value = getInt(obj, "value");
        item.flag = getVal(obj, "flag");
        item.aliases = getArr(obj, "aliases");
        items[id] = item;
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
  return items;
}

std::map<std::string, Dialogue> Loader::loadDialogues(const std::string& path) {
  std::map<std::string, Dialogue> dialogues;
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
        Dialogue d;
        d.text = getVal(obj, "text");

        size_t opts = obj.find("\"options\"");
        if (opts != std::string::npos) {
          size_t arr = obj.find('[', opts);
          int adepth = 0;
          size_t arrEnd = arr;
          for (size_t k = arr; k < obj.size(); k++) {
            if (obj[k] == '[') adepth++;
            if (obj[k] == ']') {
              adepth--;
              if (adepth == 0) {
                arrEnd = k;
                break;
              }
            }
          }

          std::string optStr = obj.substr(arr + 1, arrEnd - arr - 1);

          size_t j = 0;
          while (j < optStr.size()) {
            size_t obrace = optStr.find('{', j);
            if (obrace == std::string::npos) break;

            int odepth = 0;
            size_t oend = obrace;
            for (size_t k = obrace; k < optStr.size(); k++) {
              if (optStr[k] == '{') odepth++;
              if (optStr[k] == '}') {
                odepth--;
                if (odepth == 0) {
                  oend = k;
                  break;
                }
              }
            }

            std::string oobj = optStr.substr(obrace, oend - obrace + 1);

            DialogueOption opt;
            opt.text = getVal(oobj, "text");
            opt.flag = getVal(oobj, "flag");
            opt.next = getVal(oobj, "next");
            if (opt.flag == "null") opt.flag = "";
            if (opt.next == "null") opt.next = "";
            d.options.push_back(opt);

            j = oend + 1;
          }
        }
        dialogues[id] = d;
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
  return dialogues;
}
