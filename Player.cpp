#include "Player.h"
#include "Enemy.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

static const json& player_data() {
    static const json data = [] {
        ifstream file("Player_data.json");
        if (!file) throw runtime_error("Could not open Player_data.json");
        json j;
        file >> j;
        return j;
        }();
    return data;
}

string Player::class_to_string(PlayerClass pc) {
    switch (pc) {
    case PlayerClass::Warrior: return "Warrior";
    case PlayerClass::Knight: return "Knight";
    case PlayerClass::Rogue: return "Rogue";
    case PlayerClass::Paladin: return "Paladin";
    case PlayerClass::Berserker: return "Berserker";
    case PlayerClass::Monk: return "Monk";
    case PlayerClass::Assassin: return "Assassin";
    case PlayerClass::Duelist: return "Duelist";
    case PlayerClass::Hunter: return "Hunter";
    default: return "Unknown";
    }
}
Player::PlayerClass Player::string_to_class(const string& name) {
    if (name == "Warrior") return PlayerClass::Warrior;
    if (name == "Knight") return PlayerClass::Knight;
    if (name == "Rogue") return PlayerClass::Rogue;
    if (name == "Paladin") return PlayerClass::Paladin;
    if (name == "Berserker") return PlayerClass::Berserker;
    if (name == "Monk") return PlayerClass::Monk;
    if (name == "Assassin") return PlayerClass::Assassin;
    if (name == "Duelist") return PlayerClass::Duelist;
    if (name == "Hunter") return PlayerClass::Hunter;
    throw runtime_error("Unknown player class: " + name);
}
// A free function so the constructor can use it in its initializer list.
static const json& player_entry(Player::PlayerClass pc) {
    return player_data().at(Player::class_to_string(pc));
}

// Weapon has no default constructor, so it has to be built in the initializer list
// (the part after the colon), before the constructor body runs.
Player::Player(PlayerClass pc)
    : type(pc),
      weapon(Weapon::string_to_type(player_entry(pc).at("WeaponType").get<string>())) {
    const json& p = player_entry(pc);
    health = p.at("Health");
    // MaxHealth is optional in the JSON; older data files only have Health.
    max_health = p.contains("MaxHealth") ? p.at("MaxHealth").get<int>() : health;
    if (max_health < 1) max_health = 1;
    attack_power = p.at("AttackPower");
    armor = p.at("Armor");
    dodge_chance = p.at("DodgeChance");
    level = p.at("Level");
    gold = p.at("Gold");
    experience = 0;
    bonus_health = weapon.getHealthBonus();
    bonus_attack_power = weapon.getAttackBonus();
    bonus_armor = weapon.getArmorBonus();
    bonus_dodge_chance = weapon.getDodgeBonus();
    if (health > get_effective_max_health()) health = get_effective_max_health();
}

Player::PlayerClass Player::getType() const {
    return type;
}
string Player::Type_to_string() const {
    return class_to_string(type);
}
int Player::get_health() const {
    return health;
}
int Player::get_max_health() const {
    return max_health;
}
int Player::get_effective_max_health() const {
    int cap = max_health + bonus_health;
    return cap < 1 ? 1 : cap;
}
int Player::get_bonus_health() const {
    return bonus_health;
}
int Player::get_attack_power() const {
    return attack_power;
}
int Player::get_bonus_attack_power() const {
    return bonus_attack_power;
}
int Player::get_armor() const {
    return armor;
}
int Player::get_bonus_armor() const {
    return bonus_armor;
}
int Player::get_dodge_chance() const {
    return dodge_chance;
}
int Player::get_bonus_dodge_chance() const {
    return bonus_dodge_chance;
}
int Player::get_level() const {
    return level;
}
int Player::get_experience() const {
    return experience;
}
int Player::get_gold() const {
    return gold;
}
const Weapon& Player::get_weapon() const {
    return weapon;
}
void Player::set_type(PlayerClass pc) {
    type = pc;
}
void Player::set_health(int h) {
    // Clamp overheal-style values to the effective cap; the floor is left
    // alone so take_damage()'s death flow (health <= 0) keeps working.
    int cap = get_effective_max_health();
    health = (h > cap) ? cap : h;
}
void Player::set_max_health(int mh) {
    max_health = (mh < 1) ? 1 : mh;
    int cap = get_effective_max_health();
    if (health > cap) health = cap;
}
void Player::set_bonus_health(int bh) {
    bonus_health = bh;
    // A smaller bonus lowers the effective cap, so trim current HP to fit.
    int cap = get_effective_max_health();
    if (health > cap) health = cap;
}
void Player::set_attack_power(int ap) {
    attack_power = ap;
}
void Player::set_bonus_attack_power(int bap) {
    bonus_attack_power = bap;
}
void Player::set_armor(int a) {
    armor = a;
}
void Player::set_bonus_armor(int ba) {
    bonus_armor = ba;
}
void Player::set_dodge_chance(int d) {
    dodge_chance = d;
}
void Player::set_bonus_dodge_chance(int bd) {
    bonus_dodge_chance = bd;
}
void Player::set_level(int l) {
    level = l;
}
void Player::set_experience(int exp) {
    experience = exp;
}
void Player::set_gold(int g) {
    gold = g;
}
void Player::equip_weapon(const Weapon& w) {
    weapon = w;
}
void Player::display_stats() const {
    cout << "____________________________________________" << endl;
    cout << "Player Stats (" << Type_to_string() << "):" << endl;
    cout << "Health: " << health << " / " << get_effective_max_health()
         << " (base max " << max_health << " + bonus " << bonus_health << ")" << endl;
    cout << "Attack Power: " << attack_power << " + " << bonus_attack_power
         << " = " << (attack_power + bonus_attack_power) << endl;
    cout << "Armor: " << armor << " + " << bonus_armor
         << " = " << (armor + bonus_armor) << endl;
    cout << "Level: " << level;
    if (level >= MAX_LEVEL) {
        cout << " (MAX)";
    } else {
        cout << " (XP: " << experience << " / " << experience_to_next_level(level) << " to next)";
    }
    cout << endl;
    cout << "Experience: " << experience << endl;
    cout << "Gold: " << gold << endl;
    cout << "Dodge Chance: " << dodge_chance << "% + " << bonus_dodge_chance
         << "% = " << (dodge_chance + bonus_dodge_chance) << "%" << endl;
    cout << "Weapon Stats:" << endl;
    weapon.display_stats();
    cout << "____________________________________________" << endl;
}
void Player::attack_enemy(Enemy& enemy) {
    int damage = attack_power + bonus_attack_power - enemy.get_armor();
    if (damage < 0) damage = 0;
    enemy.take_damage(damage);
    cout << "You attacked the enemy for " << damage << " damage!" << endl;
}
void Player::take_damage(int damage) {
    if (dodge_attack()) {
        return;
    }
    int total_armor = armor + bonus_armor;
    int damage_taken = damage - total_armor;
    if (damage_taken < 0) damage_taken = 0;
    health -= damage_taken;
    cout << "You took " << damage_taken << " damage!" << endl;
    if (health <= 0) {
        player_death();
    }
}
// Stepped XP curve: cheap early levels, steeper mid game, grindy late game,
// so a single kill can never trivially chain many level-ups at high level.
// Tier 1-4: 100, 150, 200, 250 | Tier 5-9: 300, 400, ..., 700 | Tier 10+: 800, 1000, ...
int Player::experience_to_next_level(int level) {
    if (level < 1) level = 1;
    if (level < 5) return 100 + (level - 1) * 50;
    if (level < 10) return 300 + (level - 5) * 100;
    return 800 + (level - 10) * 200;
}
void Player::gain_experience(int exp) {
    if (exp <= 0) return;
    if (level >= MAX_LEVEL) {
        cout << "You gained " << exp << " experience! (already max level " << MAX_LEVEL << ")" << endl;
        return;
    }
    experience += exp;
    cout << "You gained " << exp << " experience!" << endl;
    while (level < MAX_LEVEL && experience >= experience_to_next_level(level)) {
        experience -= experience_to_next_level(level);
        gain_level();
    }
    // Leftover XP past max level is banked, not lost.
}
// Swaps the old weapon's bonuses for the new weapon's, so potion boosts on top of them are kept.
void Player::change_weapon(const Weapon& new_weapon) {
    bonus_health -= weapon.getHealthBonus();
    bonus_attack_power -= weapon.getAttackBonus();
    bonus_armor -= weapon.getArmorBonus();
    bonus_dodge_chance -= weapon.getDodgeBonus();
    weapon = new_weapon;
    cout << "You equipped a new weapon: " << weapon.get_name() << endl;
    bonus_health += weapon.getHealthBonus();
    bonus_attack_power += weapon.getAttackBonus();
    bonus_armor += weapon.getArmorBonus();
    bonus_dodge_chance += weapon.getDodgeBonus();
}
// Potions add to the bonus stats, so resetting them to just the weapon's bonuses removes the potion boosts.
void Player::clear_temporary_effects() {
    bonus_health = weapon.getHealthBonus();
    bonus_attack_power = weapon.getAttackBonus();
    bonus_armor = weapon.getArmorBonus();
    bonus_dodge_chance = weapon.getDodgeBonus();
}
void Player::gain_gold(int amount) {
    gold += amount;
    cout << "You gained " << amount << " gold!" << endl;
}
void Player::lose_gold(int amount) {
    gold -= amount;
    if (gold < 0) gold = 0;
    cout << "You lost " << amount << " gold!" << endl;
}
void Player::lose_experience(int amount) {
    experience -= amount;
    if (experience < 0) experience = 0;
    cout << "You lost " << amount << " experience!" << endl;
}
void Player::player_death() {
    // NOTE: XP/gold death penalties used to live here, but Game::run
    // discards the whole Player object on death (game over), so any penalty
    // applied here never mattered. Penalties were removed instead of being
    // left as misleading dead code; death handling (game-over flow) lives in
    // Game/Combat. This hook is kept so the public API stays stable.
    cout << "You have died!" << endl;
}
void Player::gain_level() {
    // Stepped, diminishing gains: full early, halved mid, minimal past 10,
    // with a hard cap at MAX_LEVEL so stats can't scale forever.
    if (level >= MAX_LEVEL) return;
    int hp_gain, atk_gain, armor_gain, dodge_gain;
    if (level < 5) {
        hp_gain = 20; atk_gain = 5; armor_gain = 2; dodge_gain = 1;
    } else if (level < 10) {
        hp_gain = 12; atk_gain = 3; armor_gain = 1; dodge_gain = 1;
    } else {
        hp_gain = 8; atk_gain = 1; armor_gain = 1; dodge_gain = 0;
    }
    level++;
    max_health += hp_gain;
    health += hp_gain; // level-up restores the HP it grants
    int cap = get_effective_max_health();
    if (health > cap) health = cap;
    attack_power += atk_gain;
    armor += armor_gain;
    // Keep total dodge (base + bonus) from growing without bound.
    if (dodge_gain > 0 && (dodge_chance + bonus_dodge_chance) < 40) {
        dodge_chance += dodge_gain;
    }
    cout << "You leveled up! You are now level " << level << "!" << endl;
}
void Player::use_item(const Item& item) {
    item.apply_effect(*this);
    cout << "You used " << item.get_name() << "!" << endl;
}
void Player::heal(int amount) {
    if (amount <= 0) return;
    int cap = get_effective_max_health();
    int missing = cap - health;
    if (missing <= 0) {
        cout << "You are already at full health!" << endl;
        return;
    }
    int actual = (amount > missing) ? missing : amount;
    health += actual;
    cout << "You healed " << actual << " health!";
    if (actual < amount) cout << " (capped at max " << cap << ")";
    cout << endl;
}
void Player::increase_attack_power(int amount) {
    attack_power += amount;
    cout << "Your attack power increased by " << amount << "!" << endl;
}
void Player::increase_armor(int amount) {
    armor += amount;
    cout << "Your armor increased by " << amount << "!" << endl;
}
void Player::increase_dodge_chance(int amount) {
    dodge_chance += amount;
    cout << "Your dodge chance increased by " << amount << "%!" << endl;
}
bool Player::dodge_attack() {
    if (roll_chance(dodge_chance + bonus_dodge_chance)) {
        cout << "You dodged the enemy's attack!" << endl;
        return true;
    }
    return false;
}
bool Player::is_alive() const {
    return health > 0;
}