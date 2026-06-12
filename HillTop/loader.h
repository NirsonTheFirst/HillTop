#ifndef LOADER_H
#define LOADER_H

#include <map>
#include <string>
#include <vector>

#include "player.h"

struct Location {
  std::string id, name, description;
  std::vector<std::string> exits, aliases, items, npcs;
};

class Loader {
 public:
  static std::map<std::string, std::string> loadTexts(const std::string& path);
  static std::map<std::string, Location> loadLocations(const std::string& path);
  static std::map<std::string, Item> loadItems(const std::string& path);
  static std::map<std::string, NPC> loadNPCs(const std::string& path);
  static std::map<std::string, Dialogue> loadDialogues(const std::string& path);

 private:
  static std::string getVal(const std::string& obj, const std::string& key);
  static std::vector<std::string> getArr(const std::string& obj,
                                         const std::string& key);
  static int getInt(const std::string& obj, const std::string& key);
};

#endif
