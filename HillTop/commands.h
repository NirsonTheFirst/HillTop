#ifndef COMMANDS_H
#define COMMANDS_H

#include <map>
#include <string>

class Commands {
 public:
  void load(const std::string& path);

  bool is(const std::string& input, const std::string& cmd) const;
  std::string arg(const std::string& input, const std::string& cmd) const;
  const std::string& t(const std::string& key) const;

 private:
  std::map<std::string, std::string> texts;
  static const std::string empty;
};

#endif
