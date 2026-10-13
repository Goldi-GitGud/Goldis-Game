#include "Weapon.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// Data design (see Weapon_data.json): each weapon is one archetype, no two
// share a profile. Baseline Sword (balanced 20/30/5/3), Mace (tank),
// Dagger/Saber (dodge), Spear/Axe (bruiser), Katana/Twinblade (glass),
// Greatsword/Greataxe (heavy, top attack).
static const json& weapon_data() {
    static const json data = [] {
        ifstream file("data/Weapon_data.json");
        if (!file) throw runtime_error("Could not open data/Weapon_data.json");
        json j;
        try {
            file >> j;
        } catch (const json::parse_error& e) {
            throw runtime_error(string("Weapon_data.json: invalid JSON: ") + e.what());
        }
        return j;
        }();
    return data;
}
Weapon::Weapon(WeaponType wt) : type(wt) {
    // A missing entry is a data bug: fail loudly naming the key. Missing
    // *fields* default to 0 so new bonus fields can be added to the JSON
    // without breaking old files.
    const string key = Type_to_string(wt);
    auto entry = weapon_data().find(key);
    if (entry == weapon_data().end())
        throw runtime_error("Weapon_data.json: missing entry for \"" + key + "\"");
    health_bonus = entry->value("HealthBonus", 0);
    attack_bonus = entry->value("AttackBonus", 0);
    armor_bonus = entry->value("ArmorBonus", 0);
    dodge_bonus = entry->value("DodgeBonus", 0);
}
Weapon::WeaponType Weapon::getType() const {
    return type;
}
int Weapon::getHealthBonus() const {
    return health_bonus;
}
int Weapon::getAttackBonus() const {
    return attack_bonus;
}
int Weapon::getArmorBonus() const {
    return armor_bonus;
}
int Weapon::getDodgeBonus() const {
    return dodge_bonus;
}
string Weapon::Type_to_string(WeaponType wt) const {
    switch (wt) {
    case WeaponType::Sword: return "Sword";
    case WeaponType::Greatsword: return "Greatsword";
    case WeaponType::Dagger: return "Dagger";
    case WeaponType::Spear: return "Spear";
    case WeaponType::Axe: return "Axe";
    case WeaponType::Mace: return "Mace";
    case WeaponType::Greataxe: return "Greataxe";
    case WeaponType::Katana: return "Katana";
    case WeaponType::Twinblade: return "Twinblade";
    case WeaponType::Saber: return "Saber";
    default: return "Unknown";
    }
}
// is static so it can be called without a Weapon object, used by Player constructor to build the weapon from JSON
Weapon::WeaponType Weapon::string_to_type(const string& name) {
    if (name == "Sword") return WeaponType::Sword;
    if (name == "Greatsword") return WeaponType::Greatsword;
    if (name == "Dagger") return WeaponType::Dagger;
    if (name == "Spear") return WeaponType::Spear;
    if (name == "Axe") return WeaponType::Axe;
    if (name == "Mace") return WeaponType::Mace;
    if (name == "Greataxe") return WeaponType::Greataxe;
    if (name == "Katana") return WeaponType::Katana;
    if (name == "Twinblade") return WeaponType::Twinblade;
    if (name == "Saber") return WeaponType::Saber;
    throw runtime_error("Unknown weapon name: " + name);
}
string Weapon::get_name() const {
    return Type_to_string(type);
}
string Weapon::get_effect_text() const {
    string parts;
    auto add = [&parts](const string& p) {
        if (!parts.empty()) parts += ", ";
        parts += p;
    };
    if (health_bonus != 0) add("+" + to_string(health_bonus) + " HP");
    if (attack_bonus != 0) add("+" + to_string(attack_bonus) + " Attack");
    if (armor_bonus != 0) add("+" + to_string(armor_bonus) + " Armor");
    if (dodge_bonus != 0) add("+" + to_string(dodge_bonus) + "% Dodge");
    if (parts.empty()) return "No bonuses";
    return parts;
}
void Weapon::display_stats() const {
    cout << "  Weapon: " << get_name() << endl;
    cout << "  Health Bonus: " << health_bonus << endl;
    cout << "  Attack Bonus: " << attack_bonus << endl;
    cout << "  Armor Bonus: " << armor_bonus << endl;
    cout << "  Dodge Bonus: " << dodge_bonus << endl;
}
void Weapon::setHealthBonus(int bonus) {
    health_bonus = bonus;
}
void Weapon::setAttackBonus(int bonus) {
    attack_bonus = bonus;
}
void Weapon::setArmorBonus(int bonus) {
    armor_bonus = bonus;
}
void Weapon::setDodgeBonus(int bonus) {
    dodge_bonus = bonus;
}