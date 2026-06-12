#include "inventory.h"

#include <algorithm>
#include <iostream>

#include "commands.h"

Inventory::Inventory(std::map<std::string, Item>& i, Player& p,
                     const Commands& c)
    : items(i), player(p), cmd(c) {}

Item* Inventory::find(const std::string& name) {
  for (auto& pair : items) {
    if (pair.second.name == name) return &pair.second;
    for (const auto& alias : pair.second.aliases)
      if (alias == name) return &pair.second;
  }
  return nullptr;
}

void Inventory::take(const std::string& name,
                     std::vector<std::string>& locItems) {
  for (size_t i = 0; i < locItems.size(); i++) {
    auto it = items.find(locItems[i]);
    if (it == items.end()) continue;
    Item& item = it->second;
    if (item.name == name || std::find(item.aliases.begin(), item.aliases.end(),
                                       name) != item.aliases.end()) {
      player.inventory.push_back(item);
      std::cout << cmd.t("taken") << item.name << std::endl;
      locItems.erase(locItems.begin() + i);
      return;
    }
  }
  std::cout << cmd.t("not_found") << std::endl;
}

void Inventory::show() const {
  if (player.inventory.empty()) {
    std::cout << cmd.t("inventory_empty") << std::endl;
    return;
  }
  std::cout << cmd.t("inventory") << std::endl;
  for (size_t i = 0; i < player.inventory.size(); i++)
    std::cout << "  " << (i + 1) << ". " << player.inventory[i].name
              << std::endl;
}

void Inventory::use(const std::string& name,
                    std::map<std::string, bool>& flags) {
  for (size_t i = 0; i < player.inventory.size(); i++) {
    Item& item = player.inventory[i];
    if (item.name == name || std::find(item.aliases.begin(), item.aliases.end(),
                                       name) != item.aliases.end()) {
      std::cout << cmd.t("used") << item.name << std::endl;
      if (item.type == "consumable") {
        player.hp = std::min(player.hp + item.value, player.maxHp);
        player.inventory.erase(player.inventory.begin() + i);
        return;
      }
      if (item.type == "story") {
        flags[item.flag] = true;
        return;
      }
      if (item.type == "weapon") {
        std::cout << "  Damage: " << item.damage << ", Ammo: " << item.ammo
                  << std::endl;
        return;
      }
    }
  }
  std::cout << cmd.t("cant_use") << std::endl;
}
