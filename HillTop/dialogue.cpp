#include "dialogue.h"

#include <algorithm>
#include <iostream>

#include "commands.h"

DialogueManager::DialogueManager(std::map<std::string, NPC>& n,
                                 std::map<std::string, Dialogue>& d,
                                 const Commands& c)
    : npcs(n), dialogues(d), cmd(c) {}

void DialogueManager::talk(const std::string& name,
                           const std::vector<std::string>& locNPCs,
                           std::map<std::string, bool>& flags) {
  for (const auto& npcId : locNPCs) {
    auto it = npcs.find(npcId);
    if (it == npcs.end()) continue;
    NPC& npc = it->second;
    if (npc.name == name || std::find(npc.aliases.begin(), npc.aliases.end(),
                                      name) != npc.aliases.end()) {
      runDialogue(npc.dialogue, flags);
      return;
    }
  }
  std::cout << cmd.t("npc_not_found") << std::endl;
}

void DialogueManager::runDialogue(const std::string& dialogueId,
                                  std::map<std::string, bool>& flags) {
  auto it = dialogues.find(dialogueId);
  if (it == dialogues.end()) return;
  Dialogue& d = it->second;

  std::cout << std::endl << d.text << std::endl << std::endl;
  if (d.options.empty()) return;

  std::vector<int> valid;
  for (size_t i = 0; i < d.options.size(); i++) {
      valid.push_back((int)i);
      std::cout << "  " << valid.size() << ". "
          << d.options[i].text << std::endl;
  }
  std::cout << std::endl;
  if (valid.empty()) return;

  std::cout << "> ";
  std::string input;
  std::getline(std::cin, input);
  if (input.empty()) return;

  try {
    int choice = std::stoi(input) - 1;
    if (choice >= 0 && choice < (int)valid.size()) {
      DialogueOption& opt = d.options[valid[choice]];
      if (!opt.flag.empty()) flags[opt.flag] = true;
      if (!opt.next.empty()) runDialogue(opt.next, flags);
    }
  } catch (...) {
  }
}
