#ifndef LOADER_H
#define LOADER_H

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "types.h"

class Loader {
 public:
  static Player loadPlayer(const std::string& path);
  static std::string extractObject(const std::string& text, size_t pos);
  static std::map<std::string, Location> loadLocations(const std::string& path);
  static std::map<std::string, Item> loadItems(const std::string& path);
  static std::map<std::string, NPC> loadNPCs(const std::string& path);
  static std::map<std::string, Dialogue> loadDialogues(const std::string& path);
  static std::map<std::string, std::string> loadStrings(
      const std::string& path);

 private:
  static std::string readFile(const std::string& path);
  static std::string getString(const std::string& obj, const std::string& key);
  static std::vector<std::string> getArray(const std::string& obj,
                                           const std::string& key);
  static int getInt(const std::string& obj, const std::string& key);
  static void forEachObject(
      const std::string& text,
      const std::function<void(const std::string&, const std::string&)>& cb);
};

#endif
