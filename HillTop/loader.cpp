#include "loader.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::string Loader::readFile(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "ERROR: Cannot open " << path << std::endl;
    return "";
  }
  std::stringstream buf;
  buf << file.rdbuf();
  return buf.str();
}

std::string Loader::getString(const std::string& obj, const std::string& key) {
  size_t p = obj.find("\"" + key + "\"");
  if (p == std::string::npos) return "";
  size_t c = obj.find(':', p);
  size_t q1 = obj.find('"', c + 1);
  size_t q2 = obj.find('"', q1 + 1);
  return obj.substr(q1 + 1, q2 - q1 - 1);
}

std::vector<std::string> Loader::getArray(const std::string& obj,
                                          const std::string& key) {
  std::vector<std::string> res;
  size_t p = obj.find("\"" + key + "\"");
  if (p == std::string::npos) return res;
  size_t b = obj.find('[', p), e = obj.find(']', b);
  if (b == std::string::npos || e == std::string::npos) return res;
  std::string s = obj.substr(b + 1, e - b - 1);
  for (size_t q = 0;;) {
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
  size_t s = c + 1;
  while (s < obj.size() && (obj[s] == ' ' || obj[s] == '\n')) s++;
  if (s >= obj.size() || obj[s] == '"' || obj[s] == '}' || obj[s] == ']')
    return 0;
  size_t e = s;
  while (e < obj.size() && obj[e] >= '0' && obj[e] <= '9') e++;
  if (e == s) return 0;
  return std::stoi(obj.substr(s, e - s));
}

void Loader::forEachObject(
    const std::string& text,
    const std::function<void(const std::string&, const std::string&)>& cb) {
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
        cb(id, obj);
        id.clear();
        obj.clear();
      }
    } else if (depth == 1 && c == '"' && id.empty()) {
      size_t q2 = text.find('"', i + 1);
      if (q2 == std::string::npos) break;
      id = text.substr(i + 1, q2 - i - 1);
      i = q2;
    } else if (!id.empty() && !obj.empty()) {
      obj += c;
    }
  }
}

std::map<std::string, Location> Loader::loadLocations(const std::string& path) {
  std::map<std::string, Location> map;
  std::string text = readFile(path);
  if (text.empty()) return map;
  forEachObject(text, [&](const std::string& id, const std::string& obj) {
    Location loc;
    loc.id = id;
    loc.name = getString(obj, "name");
    loc.description = getString(obj, "description");
    loc.exits = getArray(obj, "exits");
    loc.aliases = getArray(obj, "aliases");
    loc.items = getArray(obj, "items");
    loc.npcs = getArray(obj, "npcs");
    map[id] = loc;
  });
  return map;
}

std::map<std::string, NPC> Loader::loadNPCs(const std::string& path) {
  std::map<std::string, NPC> map;
  std::string text = readFile(path);
  if (text.empty()) return map;
  forEachObject(text, [&](const std::string& id, const std::string& obj) {
    NPC npc;
    npc.id = id;
    npc.name = getString(obj, "name");
    npc.description = getString(obj, "description");
    npc.dialogue = getString(obj, "dialogue_id");
    npc.aliases = getArray(obj, "aliases");
    map[id] = npc;
  });
  return map;
}

std::map<std::string, Item> Loader::loadItems(const std::string& path) {
  std::map<std::string, Item> map;
  std::string text = readFile(path);
  if (text.empty()) return map;
  forEachObject(text, [&](const std::string& id, const std::string& obj) {
    Item item;
    item.id = id;
    item.name = getString(obj, "name");
    item.description = getString(obj, "description");
    item.type = getString(obj, "type");
    item.damage = getInt(obj, "damage");
    item.ammo = getInt(obj, "ammo");
    item.value = getInt(obj, "value");
    item.flag = getString(obj, "flag");
    item.aliases = getArray(obj, "aliases");
    map[id] = item;
  });
  return map;
}

std::map<std::string, Dialogue> Loader::loadDialogues(const std::string& path) {
  std::map<std::string, Dialogue> map;
  std::string text = readFile(path);
  if (text.empty()) return map;
  forEachObject(text, [&](const std::string& id, const std::string& obj) {
    Dialogue d;
    d.text = getString(obj, "text");

    size_t optsKey = obj.find("\"options\"");
    if (optsKey != std::string::npos) {
      size_t colon = obj.find(':', optsKey);
      size_t arr = obj.find('[', colon);
      if (arr != std::string::npos) {
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
          size_t ob = optStr.find('{', j);
          if (ob == std::string::npos) break;

          int odepth = 0;
          size_t oe = ob;
          for (size_t k = ob; k < optStr.size(); k++) {
            if (optStr[k] == '{') odepth++;
            if (optStr[k] == '}') {
              odepth--;
              if (odepth == 0) {
                oe = k;
                break;
              }
            }
          }

          std::string oobj = optStr.substr(ob, oe - ob + 1);

          DialogueOption opt;
          opt.text = getString(oobj, "text");
          opt.flag = getString(oobj, "flag");
          opt.next = getString(oobj, "next");
          if (opt.flag == "null") opt.flag.clear();
          if (opt.next == "null") opt.next.clear();
          d.options.push_back(opt);

          j = oe + 1;
        }
      }
    }
    map[id] = d;
  });
  return map;
}

std::map<std::string, std::string> Loader::loadStrings(
    const std::string& path) {
  std::map<std::string, std::string> map;
  std::string text = readFile(path);
  if (text.empty()) return map;
  forEachObject(text, [&](const std::string& id, const std::string& obj) {
    map[id] = getString(obj, "text");
  });
  return map;
}
