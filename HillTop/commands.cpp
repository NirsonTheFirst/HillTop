#include "commands.h"

#include <fstream>
#include <sstream>

const std::string Commands::empty;

void Commands::load(const std::string& path) {
  std::ifstream file(path);
  std::stringstream buf;
  buf << file.rdbuf();
  std::string text = buf.str();

  size_t pos = 0;
  while (true) {
    size_t q1 = text.find('"', pos);
    if (q1 == std::string::npos) break;
    size_t q2 = text.find('"', q1 + 1);
    std::string key = text.substr(q1 + 1, q2 - q1 - 1);
    size_t c = text.find(':', q2);
    size_t v1 = text.find('"', c + 1);
    size_t v2 = text.find('"', v1 + 1);
    texts[key] = text.substr(v1 + 1, v2 - v1 - 1);
    pos = v2 + 1;
  }
}

bool Commands::is(const std::string& input, const std::string& cmd) const {
  auto it = texts.find(cmd);
  return it != texts.end() && input == it->second;
}

std::string Commands::arg(const std::string& input,
                          const std::string& cmd) const {
  auto it = texts.find(cmd);
  if (it == texts.end()) return "";
  const std::string& c = it->second;
  return (input.find(c) == 0) ? input.substr(c.size()) : "";
}

const std::string& Commands::t(const std::string& key) const {
  auto it = texts.find(key);
  return it != texts.end() ? it->second : empty;
}
