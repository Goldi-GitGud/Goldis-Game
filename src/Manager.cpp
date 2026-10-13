#include "Manager.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <exception>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// Like the data JSONs, this is a relative path (relative to where the program is run from).
string Manager::slot_path(int slot) {
    return "save_slot_" + to_string(slot) + ".json";
}

bool Manager::save_game_saves(const Game& game, int slot) const {
    if (!game.has_game()) {
        cout << "There is no game to save." << endl;
        return false;
    }
    // Overwrite confirm (T8): never silently clobber an existing slot.
    {
        ifstream probe(slot_path(slot));
        if (probe) {
            cout << "Slot " << slot << " already holds: " << describe_slot(slot) << endl;
            cout << "Overwrite?" << endl;
            cout << "1. Yes" << endl;
            cout << "2. No" << endl;
            cout << "Enter your choice: ";
            if (read_choice(1, 2) == 2) {
                cout << "Save cancelled." << endl;
                return false;
            }
        }
    }
    const Player& p = game.get_player();

    json j;
    j["Class"] = Player::class_to_string(p.getType());
    j["Level"] = p.get_level();
    j["Experience"] = p.get_experience();
    j["Gold"] = p.get_gold();
    j["Health"] = p.get_health();
    // base stats are saved because level-ups change them; bonus stats are not, they're rebuilt from the weapon
    j["AttackPower"] = p.get_attack_power();
    j["Armor"] = p.get_armor();
    j["DodgeChance"] = p.get_dodge_chance();
    // T8 additions. Bonus stats are saved so a future max-HP cap (T4) has its
    // inputs; loading still rebuilds them from the weapon when keys are absent
    // (old saves keep loading). Difficulty is global and applies on load.
    j["Difficulty"] = game.get_difficulty();
    j["BonusHealth"] = p.get_bonus_health();
    j["BonusAttackPower"] = p.get_bonus_attack_power();
    j["BonusArmor"] = p.get_bonus_armor();
    j["BonusDodgeChance"] = p.get_bonus_dodge_chance();
    j["Weapon"] = p.get_weapon().get_name();
    j["Items"] = json::array();
    for (const Item& item : game.get_bag().getItems()) {
        j["Items"].push_back(item.get_name());
    }
    j["Weapons"] = json::array();
    for (const Weapon& weapon : game.get_bag().getWeapons()) {
        j["Weapons"].push_back(weapon.get_name());
    }
    j["Wave"] = game.get_wave();

    ofstream out(slot_path(slot));
    if (!out) {
        cout << "Could not write " << slot_path(slot) << endl;
        return false;
    }
    out << j.dump(4);
    out.close();
    if (!out) {
        cout << "Something went wrong while writing " << slot_path(slot) << endl;
        return false;
    }
    cout << "Game saved to slot " << slot << "." << endl;
    // Honest mid-wave policy (T8 + DESIGN.md #8): enemies/wave_active are NOT
    // persisted, so a save taken mid-fight reloads at the same wave number but
    // restarts that wave fresh. Camp saves are unaffected.
    if (game.is_mid_wave()) {
        cout << "Note: saved in the middle of wave " << game.get_wave()
             << " - loading will restart that wave fresh (enemies are not saved)." << endl;
    }
    return true;
}

bool Manager::load_game_saves(Game& game, int slot) const {
    ifstream in(slot_path(slot));
    if (!in) {
        cout << "Slot " << slot << " is empty." << endl;
        return false;
    }
    try {
        json j;
        in >> j;

        Player p(Player::string_to_class(j.at("Class").get<string>()));
        p.set_level(j.at("Level").get<int>());
        p.set_experience(j.at("Experience").get<int>());
        p.set_gold(j.at("Gold").get<int>());
        p.set_health(j.at("Health").get<int>());
        p.set_attack_power(j.at("AttackPower").get<int>());
        p.set_armor(j.at("Armor").get<int>());
        p.set_dodge_chance(j.at("DodgeChance").get<int>());
        p.equip_weapon(Weapon(Weapon::string_to_type(j.at("Weapon").get<string>())));
        // New keys use value() with fallbacks so pre-T8 saves still load.
        // Bonus stats equal the weapon bonuses at camp time, so rebuilding is identical.
        if (j.contains("BonusHealth")) {
            p.set_bonus_health(j.value("BonusHealth", p.get_bonus_health()));
            p.set_bonus_attack_power(j.value("BonusAttackPower", p.get_bonus_attack_power()));
            p.set_bonus_armor(j.value("BonusArmor", p.get_bonus_armor()));
            p.set_bonus_dodge_chance(j.value("BonusDodgeChance", p.get_bonus_dodge_chance()));
        } else {
            p.clear_temporary_effects(); // rebuilds the bonus stats from the equipped weapon
        }

        Bag b;
        for (const json& name : j.at("Items")) {
            b.addItem(Item(Item::string_to_type(name.get<string>())));
        }
        for (const json& name : j.at("Weapons")) {
            b.addWeapon(Weapon(Weapon::string_to_type(name.get<string>())));
        }

        // Only touch the game once everything above has worked, so a broken save can't leave it half-loaded.
        game.load_state(p, b, j.at("Wave").get<int>());
        game.set_difficulty(j.value("Difficulty", game.get_difficulty())); // absent in pre-T8 saves: keep current
    }
    catch (const exception& e) {
        cout << "Could not load slot " << slot << " (the save file is damaged or incomplete): " << e.what() << endl;
        return false;
    }
    cout << "Game loaded from slot " << slot << "." << endl;
    return true;
}

string Manager::describe_slot(int slot) const {
    ifstream in(slot_path(slot));
    if (!in) {
        return "empty";
    }
    try {
        json j;
        in >> j;
        // value() with defaults: never throws on missing keys, so old saves,
        // new saves, and hand-edited files all get a sane one-line summary.
        if (!j.is_object()) {
            return "damaged save file";
        }
        int level = j.value("Level", 0);
        int wave = j.value("Wave", 0);
        if (level <= 0 || wave <= 0) {
            return "damaged save file";
        }
        string text = j.value("Class", string("Unknown")) + ", level " + to_string(level)
            + ", wave " + to_string(wave);
        if (j.contains("Difficulty")) {
            text += ", difficulty " + to_string(j.value("Difficulty", 0));
        }
        return text;
    }
    catch (const exception&) {
        return "damaged save file";
    }
}