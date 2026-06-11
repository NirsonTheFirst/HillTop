#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>

struct Location {
  std::string id;
  std::string name;
  std::string description;
  std::vector<std::string> exits;
  std::vector<std::string> aliases;
};

#endif
