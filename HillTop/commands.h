#ifndef COMMANDS_H
#define COMMANDS_H

#include <map>
#include <string>

class Commands {
 public:
  void load(const std::string& path);

  bool is(const std::string& input, const std::string& cmd) const;
  std::string arg(const std::string& input, const std::string& cmd) const;
  std::string t(const std::string& key) const;

 private:
  std::map<std::string, std::string> cmdMap;
  std::map<std::string, std::string> textsCache;
};

#endif
