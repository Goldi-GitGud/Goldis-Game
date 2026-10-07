#include "Item.h"
#include "Player.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

static const json& item_data() {
    static const json data = [] {
        ifstream file("Item_data.json");
        if (!file) throw runtime_error("Could not open Item_data.json");
        json j;
        file >> j;
        return j;
        }();
    return data;
}
Item::Item(ItemType it) : type(it) {
    const json& i = item_data().at(Type_to_string(it));
    health_bonus = i.at("HealthBonus");
    attack_bonus = i.at("AttackBonus");
    armor_bonus = i.at("ArmorBonus");
    dodge_bonus = i.at("DodgeBonus");
}
Item::ItemType Item::getType() const {
    return type;
}
int Item::getHealthBonus() const {
    return health_bonus;
}
int Item::getAttackBonus() const {
    return attack_bonus;
}
int Item::getArmorBonus() const {
    return armor_bonus;
}
int Item::getDodgeBonus() const {
    return dodge_bonus;
}
string Item::Type_to_string(ItemType it) const {
    switch (it) {
    case ItemType::Small_Health_potion: return "Small_Health_potion";
    case ItemType::Small_Defense_potion: return "Small_Defense_potion";
    case ItemType::Small_Dodge_potion: return "Small_Dodge_potion";
    case ItemType::Small_Attack_potion: return "Small_Attack_potion";
    case ItemType::Medium_Health_potion: return "Medium_Health_potion";
    case ItemType::Medium_Defense_potion: return "Medium_Defense_potion";
    case ItemType::Medium_Dodge_potion: return "Medium_Dodge_potion";
    case ItemType::Medium_Attack_potion: return "Medium_Attack_potion";
    case ItemType::Large_Health_potion: return "Large_Health_potion";
    case ItemType::Large_Defense_potion: return "Large_Defense_potion";
    case ItemType::Large_Dodge_potion: return "Large_Dodge_potion";
    case ItemType::Large_Attack_potion: return "Large_Attack_potion";
    default: return "Unknown";
    }
}
string Item::get_name() const {
    return Type_to_string(type);
}
Item::ItemType Item::string_to_type(const string& name) {
    if (name == "Small_Health_potion") return ItemType::Small_Health_potion;
    if (name == "Small_Defense_potion") return ItemType::Small_Defense_potion;
    if (name == "Small_Dodge_potion") return ItemType::Small_Dodge_potion;
    if (name == "Small_Attack_potion") return ItemType::Small_Attack_potion;
    if (name == "Medium_Health_potion") return ItemType::Medium_Health_potion;
    if (name == "Medium_Defense_potion") return ItemType::Medium_Defense_potion;
    if (name == "Medium_Dodge_potion") return ItemType::Medium_Dodge_potion;
    if (name == "Medium_Attack_potion") return ItemType::Medium_Attack_potion;
    if (name == "Large_Health_potion") return ItemType::Large_Health_potion;
    if (name == "Large_Defense_potion") return ItemType::Large_Defense_potion;
    if (name == "Large_Dodge_potion") return ItemType::Large_Dodge_potion;
    if (name == "Large_Attack_potion") return ItemType::Large_Attack_potion;
    throw runtime_error("Unknown item name: " + name);
}
void Item::apply_effect(Player& player) const {
    if (health_bonus > 0) {
        player.heal(health_bonus);
    }
    if (attack_bonus > 0) {
        player.set_bonus_attack_power(player.get_bonus_attack_power() + attack_bonus);
        cout << "Your attack power is boosted by " << attack_bonus << " until the wave ends!" << endl;
    }
    if (armor_bonus > 0) {
        player.set_bonus_armor(player.get_bonus_armor() + armor_bonus);
        cout << "Your armor is boosted by " << armor_bonus << " until the wave ends!" << endl;
    }
    if (dodge_bonus > 0) {
        player.set_bonus_dodge_chance(player.get_bonus_dodge_chance() + dodge_bonus);
        cout << "Your dodge chance is boosted by " << dodge_bonus << "% until the wave ends!" << endl;
    }
}
void Item::setHealthBonus(int bonus) {
    health_bonus = bonus;
}
void Item::setAttackBonus(int bonus) {
    attack_bonus = bonus;
}
void Item::setArmorBonus(int bonus) {
    armor_bonus = bonus;
}
void Item::setDodgeBonus(int bonus) {
    dodge_bonus = bonus;
}