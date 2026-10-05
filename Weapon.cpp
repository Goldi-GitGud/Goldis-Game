#include "Weapon.h"
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

static const json& weapon_data() {
    static const json data = [] {
        ifstream file("Weapon_data.json");
        if (!file) throw runtime_error("Could not open Weapon_data.json");
        json j;
        file >> j;
        return j;
        }();
    return data;
}
Weapon::Weapon(WeaponType wt) : type(wt) {
    const json& w = weapon_data().at(Type_to_string());
    health_bonus = w.at("HealthBonus");
    attack_bonus = w.at("AttackBonus");
    armor_bonus = w.at("ArmorBonus");
    dodge_bonus = w.at("DodgeBonus");
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
void Weapon::display_stats() const {
    cout << "Weapon Type: " << Type_to_string(type) << endl;
    cout << "Health Bonus: " << health_bonus << endl;
    cout << "Attack Bonus: " << attack_bonus << endl;
    cout << "Armor Bonus: " << armor_bonus << endl;
    cout << "Dodge Bonus: " << dodge_bonus << endl;
}