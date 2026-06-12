#include "world.h"

#include "loader.h"

void World::load(const std::string& path) {
  locations = Loader::loadLocations(path);
}

const std::string& World::current() const { return current_id; }

Location& World::here() { return locations[current_id]; }

const Location& World::here() const { return locations.at(current_id); }

Location& World::get(const std::string& id) { return locations[id]; }

const std::map<std::string, Location>& World::all() const { return locations; }

bool World::canGo(const std::string& id) const {
  const auto& cur = locations.at(current_id);
  for (const auto& exit : cur.exits)
    if (exit == id) return true;
  return false;
}

void World::goTo(const std::string& id) { current_id = id; }

std::string World::resolve(const std::string& input) const {
  if (locations.count(input)) return input;
  for (const auto& pair : locations) {
    if (pair.second.name == input) return pair.first;
    for (const auto& alias : pair.second.aliases)
      if (alias == input) return pair.first;
  }
  return input;
}
