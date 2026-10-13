#include "Item.h"
#include "Player.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// Data design (see Item_data.json): 3 tiers (Small/Medium/Large) x 4 effects
// (Health/Attack/Armor/Dodge). Health heals immediately, the other three are
// wave-long boosts applied via apply_effect(). Item_data.json was reviewed
// for this task and left unchanged: the tiers scale cleanly
// (HP 20/40/70, Atk 10/20/35, Armor 20/35/50, Dodge 5/10/20).
static const json& item_data() {
    static const json data = [] {
        ifstream file("Item_data.json");
        if (!file) throw runtime_error("Could not open Item_data.json");
        json j;
        try {
            file >> j;
        } catch (const json::parse_error& e) {
            throw runtime_error(string("Item_data.json: invalid JSON: ") + e.what());
        }
        return j;
        }();
    return data;
}
Item::Item(ItemType it) : type(it) {
    // A missing entry is a data bug: fail loudly naming the key. Missing
    // *fields* default to 0 so new bonus fields can be added to the JSON
    // without breaking old files.
    const string key = Type_to_string(it);
    auto entry = item_data().find(key);
    if (entry == item_data().end())
        throw runtime_error("Item_data.json: missing entry for \"" + key + "\"");
    health_bonus = entry->value("HealthBonus", 0);
    attack_bonus = entry->value("AttackBonus", 0);
    armor_bonus = entry->value("ArmorBonus", 0);
    dodge_bonus = entry->value("DodgeBonus", 0);
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
string Item::get_effect_text() const {
    string parts;
    auto add = [&parts](const string& p) {
        if (!parts.empty()) parts += ", ";
        parts += p;
    };
    if (health_bonus != 0) add("Restores " + to_string(health_bonus) + " HP");
    if (attack_bonus != 0) add("+" + to_string(attack_bonus) + " Attack until wave ends");
    if (armor_bonus != 0) add("+" + to_string(armor_bonus) + " Armor until wave ends");
    if (dodge_bonus != 0) add("+" + to_string(dodge_bonus) + "% Dodge until wave ends");
    if (parts.empty()) return "No effect";
    return parts;
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