#include "loader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

static std::string readFile(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cout << "ERROR: Cannot open " << path << std::endl;
    return "";
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();
  return buffer.str();
}

static std::string getString(const std::string& obj, const std::string& key) {
  std::string search = "\"" + key + "\"";
  size_t pos = obj.find(search);
  if (pos == std::string::npos) return "";
  size_t colon = obj.find(':', pos);
  size_t q1 = obj.find('"', colon + 1);
  size_t q2 = obj.find('"', q1 + 1);
  return obj.substr(q1 + 1, q2 - q1 - 1);
}

static std::vector<std::string> getArray(const std::string& obj,
                                         const std::string& key) {
  std::vector<std::string> result;
  std::string search = "\"" + key + "\"";
  size_t pos = obj.find(search);
  if (pos == std::string::npos) return result;
  size_t bracket = obj.find('[', pos);
  size_t endBracket = obj.find(']', bracket);
  std::string content = obj.substr(bracket + 1, endBracket - bracket - 1);
  size_t q = 0;
  while (q < content.size()) {
    size_t q1 = content.find('"', q);
    if (q1 == std::string::npos) break;
    size_t q2 = content.find('"', q1 + 1);
    result.push_back(content.substr(q1 + 1, q2 - q1 - 1));
    q = q2 + 1;
  }
  return result;
}

std::map<std::string, Location> DataLoader::loadLocations(
    const std::string& filepath) {
  std::map<std::string, Location> locations;
  std::string text = readFile(filepath);
  if (text.empty()) return locations;

  int depth = 0;
  std::string currentId;
  std::string currentObj;

  for (size_t i = 0; i < text.size(); i++) {
    char c = text[i];

    if (c == '{') {
      if (depth == 1 && !currentId.empty() && currentObj.empty()) {
        currentObj = "{";
      }
      depth++;
      if (!currentId.empty() && !currentObj.empty()) {
        currentObj += c;
      }
      continue;
    }

    if (c == '}') {
      depth--;
      if (!currentId.empty() && !currentObj.empty()) {
        currentObj += c;
      }
      if (depth == 1 && !currentId.empty() && !currentObj.empty()) {
        Location loc;
        loc.id = currentId;
        loc.name = getString(currentObj, "name");
        loc.description = getString(currentObj, "description");
        loc.exits = getArray(currentObj, "exits");
        loc.aliases = getArray(currentObj, "aliases");
        locations[currentId] = loc;

        currentId = "";
        currentObj = "";
      }
      continue;
    }

    if (depth == 1 && c == '"' && currentId.empty()) {
      size_t q2 = text.find('"', i + 1);
      if (q2 == std::string::npos) break;
      currentId = text.substr(i + 1, q2 - i - 1);
      i = q2;
      continue;
    }

    if (!currentId.empty() && !currentObj.empty()) {
      currentObj += c;
    }
  }

  return locations;
}
