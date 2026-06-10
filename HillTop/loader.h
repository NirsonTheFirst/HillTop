#ifndef LOADER_H
#define LOADER_H

#include <map>
#include <string>
#include <vector>

#include "location.h"

class DataLoader {
 public:
  static std::map<std::string, Location> loadLocations(
      const std::string& filepath);
};

#endif
