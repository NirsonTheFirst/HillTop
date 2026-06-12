#include "game.h"

#include <iostream>

void Game::showHelp() {
    std::cout << cmd.t("help") << std::endl;
    std::cout << cmd.t("help_look") << std::endl;
    std::cout << cmd.t("help_go") << std::endl;
    std::cout << cmd.t("help_take") << std::endl;
    std::cout << cmd.t("help_inv") << std::endl;
    std::cout << cmd.t("help_use") << std::endl;
    std::cout << cmd.t("help_talk") << std::endl;
    std::cout << cmd.t("help_duel") << std::endl;
    std::cout << cmd.t("help_help") << std::endl;
    std::cout << cmd.t("help_quit") << std::endl;
}

void Game::run() {
    std::string input;

    while (is_running) {

        std::cout << "> ";

        std::getline(std::cin, input);

        if (input.empty())
            continue;

        if (cmd.is(input, "cmd_help")) {
            showHelp();
            continue;
        }

        if (cmd.is(input, "cmd_look")) {
            showLocation();
            continue;
        }

        if (cmd.is(input, "cmd_quit")) {
            is_running = false;
            std::cout << cmd.t("goodbye") << std::endl;
            continue;
        }

        if (cmd.is(input, "cmd_inv")) {
            showInventory();
            continue;
        }

        if (cmd.is(input, "cmd_duel")) {
            duel();
            checkEndings();
            continue;
        }

        std::string arg;

        arg = cmd.arg(input, "cmd_go");
        if (!arg.empty()) {
            moveTo(arg);
            continue;
        }

        arg = cmd.arg(input, "cmd_take");
        if (!arg.empty()) {
            takeItem(arg);
            continue;
        }

        arg = cmd.arg(input, "cmd_use");
        if (!arg.empty()) {
            useItem(arg);
            continue;
        }

        arg = cmd.arg(input, "cmd_talk");
        if (!arg.empty()) {
            talkTo(arg);
            continue;
        }

        std::cout << cmd.t("unknown") << std::endl;
    }
}