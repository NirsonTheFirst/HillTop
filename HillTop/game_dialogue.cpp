#include "game.h"

#include <algorithm>
#include <iostream>

NPC* Game::findNPC(const std::string& name) {
    for (std::map<std::string, NPC>::iterator it = npcs.begin();
        it != npcs.end(); ++it) {

        if (it->second.name == name)
            return &it->second;

        for (size_t i = 0; i < it->second.aliases.size(); i++)
            if (it->second.aliases[i] == name)
                return &it->second;
    }

    return NULL;
}

void Game::talkTo(const std::string& name) {
    Location& loc = world.here();

    for (size_t i = 0; i < loc.npcs.size(); i++) {
        std::string npcId = loc.npcs[i];

        if (!npcs.count(npcId))
            continue;

        NPC& npc = npcs[npcId];

        if (npc.name == name ||
            std::find(npc.aliases.begin(),
                npc.aliases.end(),
                name) != npc.aliases.end()) {

            runDialogue(npc.dialogue);
            return;
        }
    }

    std::cout << cmd.t("npc_not_found") << std::endl;
}

void Game::runDialogue(const std::string& dialogueId) {
    if (!dialogues.count(dialogueId))
        return;

    Dialogue& d = dialogues[dialogueId];

    std::cout << std::endl;
    std::cout << d.text << std::endl;
    std::cout << std::endl;

    if (d.options.empty())
        return;

    std::vector<int> valid;

    for (size_t i = 0; i < d.options.size(); i++) {
        std::string req = d.options[i].flag;

        if (!req.empty() && !flags[req])
            continue;

        valid.push_back((int)i);

        std::cout
            << valid.size()
            << ". "
            << d.options[i].text
            << std::endl;
    }

    std::cout << std::endl;

    if (valid.empty())
        return;

    std::cout << "> ";

    std::string input;
    std::getline(std::cin, input);

    if (input.empty())
        return;

    try {
        int choice = std::stoi(input) - 1;

        if (choice >= 0 &&
            choice < (int)valid.size()) {

            DialogueOption& opt =
                d.options[valid[choice]];

            if (!opt.flag.empty())
                flags[opt.flag] = true;

            if (!opt.next.empty())
                runDialogue(opt.next);
        }
    }
    catch (...) {
        return;
    }
}