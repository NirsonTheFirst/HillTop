#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <map>
#include <string>
#include <vector>

#include "types.h"

class Commands;

class DialogueManager {
 public:
  DialogueManager(std::map<std::string, NPC>& npcs,
                  std::map<std::string, Dialogue>& dialogues,
                  const Commands& cmd);

  void talk(const std::string& name, const std::vector<std::string>& locNPCs,
            std::map<std::string, bool>& flags);

 private:
  std::map<std::string, NPC>& npcs;
  std::map<std::string, Dialogue>& dialogues;
  const Commands& cmd;

  void runDialogue(const std::string& dialogueId,
                   std::map<std::string, bool>& flags);
};

#endif
