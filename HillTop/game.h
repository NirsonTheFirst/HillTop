#ifndef GAME_H
#define GAME_H

#include <map>
#include <string>
#include <vector>

struct Location {
  std::string id, name, description;
  std::vector<std::string> exits, aliases;
};

struct Player {
  std::string name;
};

class Game {
 public:
  Game();
  void init();
  void run();
  void shutdown();
  bool isRunning() const;

 private:
  bool is_running;
  Player player;
  std::map<std::string, Location> locations;
  std::string current_location_id;
  std::map<std::string, std::string> texts;

  void loadTexts(const std::string& path);
  void loadLocations(const std::string& path);
  void processCommand(const std::string& input);
  void showLocation();
  void moveTo(const std::string& target);
  std::string t(const std::string& key);
  bool isCmd(const std::string& in, const std::string& key);
  std::string arg(const std::string& in, const std::string& key);
  std::string getVal(const std::string& obj, const std::string& key);
  std::vector<std::string> getArr(const std::string& obj,
                                  const std::string& key);
};

#endif
