#include "commands.h"

#include "loader.h"

void Commands::load(const std::string& path) {
  // Загружаем ВСЕ тексты из Loader (ключи только там)
  std::map<std::string, std::string> allTexts = Loader::loadTexts(path);

  // Извлекаем только команды (начинаются с "cmd_")
  for (std::map<std::string, std::string>::const_iterator it = allTexts.begin();
       it != allTexts.end(); ++it) {
    const std::string& key = it->first;
    const std::string& value = it->second;

    if (key.size() >= 4 && key.substr(0, 4) == "cmd_") {
      cmdMap[key] = value;
    }
    // Сохраняем все тексты для метода t()
    allTextsCache[key] = value;
  }
}

bool Commands::is(const std::string& input, const std::string& cmd) const {
  std::map<std::string, std::string>::const_iterator it = cmdMap.find(cmd);
  return it != cmdMap.end() && input == it->second;
}

std::string Commands::arg(const std::string& input,
                          const std::string& cmd) const {
  std::map<std::string, std::string>::const_iterator it = cmdMap.find(cmd);
  if (it == cmdMap.end()) return "";
  const std::string& c = it->second;
  if (input.size() >= c.size() && input.substr(0, c.size()) == c) {
    return input.substr(c.size());
  }
  return "";
}

std::string Commands::t(const std::string& key) const {
  std::map<std::string, std::string>::const_iterator it =
      allTextsCache.find(key);
  return (it != allTextsCache.end()) ? it->second : key;
}
