#ifndef WORLD_H
#define WORLD_H

#include <map>
#include <string>

#include "types.h"

class World {
 public:
  void load(const std::string& path);

  const std::string& current() const;
  Location& here();
  const Location& here() const;
  Location& get(const std::string& id);
  const std::map<std::string, Location>& all() const;

  bool canGo(const std::string& id) const;
  void goTo(const std::string& id);
  std::string resolve(const std::string& input) const;

 private:
  std::map<std::string, Location> locations;
  std::string current_id = "bank";
};

#endif
