#ifndef TEXT_H
#define TEXT_H

#include <map>
#include <string>
#include <vector>

class Text {
 public:
  static void load(const std::string& filepath);
  static std::string get(const std::string& key);
  static bool isCommand(const std::string& input, const std::string& cmdKey);
  static std::string extractArg(const std::string& input,
                                const std::string& cmdKey);

 private:
  static std::map<std::string, std::string> strings;
};

#endif
