#include "Game.h"
#include "Utils.h"
#include <iostream>
#include <stdexcept>
using namespace std;

Game::Game() : wave(1), difficulty(2) {}

void Game::new_game(Player::PlayerClass pc) {
    combat.reset(); // Combat points at the player and bag, so it has to go first
    player.emplace(pc);
    bag = Bag();
    // placeholder starting items so "Use item" has something to use
    bag.addItem(Item(Item::ItemType::Small_Health_potion));
    bag.addItem(Item(Item::ItemType::Small_Health_potion));
    bag.addItem(Item(Item::ItemType::Small_Defense_potion));
    wave = 1;
    combat.emplace(*player, bag);
}

void Game::load_state(const Player& p, const Bag& b, int wave_number) {
    combat.reset();
    player.emplace(p);
    bag = b;
    wave = wave_number;
    combat.emplace(*player, bag);
}

bool Game::has_game() const {
    return player.has_value();
}

// ---- the menu between waves ----

bool Game::camp() {
    rested_this_camp = false; // fresh visit: the one free rest is available again
    while (true) {
        // No max-HP exists yet (T4 adds the cap); show current HP plus the
        // weapon bonus, plus everything else the camp hub needs (T6).
        // XP threshold level*100 mirrors Player::gain_experience.
        cout << "____________________________________________" << endl;
        cout << "Camp - next up: wave " << wave << endl;
        cout << player->Type_to_string() << " Lv " << player->get_level()
             << " | HP: " << player->get_health() << " (bonus +" << player->get_bonus_health() << ")"
             << " | XP: " << player->get_experience() << "/" << player->get_level() * 100
             << " | Gold: " << player->get_gold()
             << " | Weapon: " << player->get_weapon().get_name() << endl;
        cout << "1. Start wave" << endl;
        cout << "2. Use item" << endl;
        cout << "3. View my stats" << endl;
        cout << "4. View bag" << endl;
        cout << "5. Heal / Rest" << endl;
        cout << "6. Equip weapon" << endl;
        cout << "7. Shop (buy/sell)" << endl;
        cout << "8. Pause" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(1, 8);
        switch (choice) {
        case 1:
            return true;
        case 2:
            combat->use_item_menu();
            break;
        case 3:
            player->display_stats();
            break;
        case 4:
            bag.displayContents();
            break;
        case 5:
            camp_heal();
            break;
        case 6:
            camp_equip();
            break;
        case 7:
            camp_shop();
            break;
        default:
            return false;
        }
    }
}

// Camp heal choice: paid heal scales with effective max HP and is repeatable
// while gold lasts; free rest scales with level and is once per camp visit.
// At full HP nothing is charged; Player::heal caps at the max.
void Game::camp_heal() {
    while (true) {
        int cap = player->get_effective_max_health();
        int hp = player->get_health();
        int missing = cap - hp;
        if (missing <= 0) {
            cout << "You are already at full health! (" << hp << "/" << cap << ")" << endl;
            return;
        }
        int cost = 10 + cap / 10;
        int paid_amount = cap * 30 / 100;
        if (paid_amount < 20) paid_amount = 20;
        int rest_amount = 5 + player->get_level();
        cout << "____________________________________________" << endl;
        cout << "Heal (HP: " << hp << "/" << cap << ", missing " << missing << ")" << endl;
        cout << "1. Paid heal (" << cost << "g -> +" << paid_amount << " HP)" << endl;
        if (rested_this_camp) {
            cout << "2. Free rest (already rested this visit)" << endl;
        } else {
            cout << "2. Free rest (+" << rest_amount << " HP, once per visit)" << endl;
        }
        cout << "0. Back" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(0, 2);
        if (choice == 0) {
            return;
        }
        if (choice == 1) {
            if (player->get_gold() < cost) {
                cout << "Not enough gold (need " << cost << "g)." << endl;
                continue;
            }
            player->lose_gold(cost);
            player->heal(paid_amount);
        } else {
            if (rested_this_camp) {
                cout << "You already rested during this visit. Come back after the next wave." << endl;
                continue;
            }
            rested_this_camp = true;
            player->heal(rest_amount);
        }
        // Loop back so a partial heal can be topped up; the full-HP check
        // at the top ends the menu (charging nothing) once capped.
    }
}

void Game::camp_equip() {
    const vector<Weapon>& weapons = bag.getWeapons();
    if (weapons.empty()) {
        cout << "Your bag holds no spare weapons." << endl;
        return;
    }
    cout << "Equipped: " << player->get_weapon().get_name() << endl;
    for (size_t i = 0; i < weapons.size(); i++) {
        cout << i + 1 << ". " << weapons[i].get_name() << endl;
    }
    cout << "0. Back" << endl;
    cout << "Enter your choice: ";
    int choice = read_choice(0, static_cast<int>(weapons.size()));
    if (choice == 0) {
        return;
    }
    Weapon picked = weapons[static_cast<size_t>(choice) - 1];
    bag.removeWeapon(picked);
    bag.addWeapon(player->get_weapon()); // old weapon goes back to the bag
    player->change_weapon(picked);
}

// Level-gated shop stock (DESIGN.md #4): LOW = Lv<5, MID = Lv5-9, HIGH = Lv10+.
// Small potions + basic weapons always; Medium + mid weapons from Lv5;
// Large + top weapons from Lv10. Buy prices stay monotonic across tiers;
// resale is 1/3 of buy price (min 1g). No save fields; retunes need no migration.
namespace {
int shop_item_price(Item::ItemType t) {
    using IT = Item::ItemType;
    switch (t) {
    case IT::Small_Health_potion: return 15;
    case IT::Small_Attack_potion: return 15;
    case IT::Small_Defense_potion: return 10;
    case IT::Small_Dodge_potion: return 12;
    case IT::Medium_Health_potion: return 30;
    case IT::Medium_Attack_potion: return 30;
    case IT::Medium_Defense_potion: return 30;
    case IT::Medium_Dodge_potion: return 30;
    case IT::Large_Health_potion: return 60;
    case IT::Large_Attack_potion: return 60;
    case IT::Large_Defense_potion: return 60;
    case IT::Large_Dodge_potion: return 60;
    default: return -1;
    }
}
int shop_item_unlock(Item::ItemType t) {
    using IT = Item::ItemType;
    switch (t) {
    case IT::Medium_Health_potion:
    case IT::Medium_Attack_potion:
    case IT::Medium_Defense_potion:
    case IT::Medium_Dodge_potion: return 5;
    case IT::Large_Health_potion:
    case IT::Large_Attack_potion:
    case IT::Large_Defense_potion:
    case IT::Large_Dodge_potion: return 10;
    default: return 1;
    }
}
int shop_weapon_price(Weapon::WeaponType t) {
    using WT = Weapon::WeaponType;
    switch (t) {
    case WT::Sword:
    case WT::Dagger:
    case WT::Mace:
    case WT::Spear:
    case WT::Axe: return 40;
    case WT::Saber:
    case WT::Katana:
    case WT::Twinblade: return 90;
    case WT::Greatsword:
    case WT::Greataxe: return 150;
    default: return -1;
    }
}
int shop_weapon_unlock(Weapon::WeaponType t) {
    using WT = Weapon::WeaponType;
    switch (t) {
    case WT::Saber:
    case WT::Katana:
    case WT::Twinblade: return 5;
    case WT::Greatsword:
    case WT::Greataxe: return 10;
    default: return 1;
    }
}
// Resale is 1/3 of buy price (min 1g). Entries without a buy price keep the
// old flat sell values (5g item / 15g weapon).
int shop_item_sell(Item::ItemType t) {
    int buy = shop_item_price(t);
    if (buy < 0) return 5;
    int sell = buy / 3;
    return sell < 1 ? 1 : sell;
}
int shop_weapon_sell(Weapon::WeaponType t) {
    int buy = shop_weapon_price(t);
    if (buy < 0) return 15;
    int sell = buy / 3;
    return sell < 1 ? 1 : sell;
}
const Item::ItemType kShopItems[] = {
    Item::ItemType::Small_Health_potion,
    Item::ItemType::Small_Attack_potion,
    Item::ItemType::Small_Defense_potion,
    Item::ItemType::Small_Dodge_potion,
    Item::ItemType::Medium_Health_potion,
    Item::ItemType::Medium_Attack_potion,
    Item::ItemType::Medium_Defense_potion,
    Item::ItemType::Medium_Dodge_potion,
    Item::ItemType::Large_Health_potion,
    Item::ItemType::Large_Attack_potion,
    Item::ItemType::Large_Defense_potion,
    Item::ItemType::Large_Dodge_potion,
};
const Weapon::WeaponType kShopWeapons[] = {
    Weapon::WeaponType::Sword,
    Weapon::WeaponType::Dagger,
    Weapon::WeaponType::Mace,
    Weapon::WeaponType::Spear,
    Weapon::WeaponType::Axe,
    Weapon::WeaponType::Saber,
    Weapon::WeaponType::Katana,
    Weapon::WeaponType::Twinblade,
    Weapon::WeaponType::Greatsword,
    Weapon::WeaponType::Greataxe,
};
} // namespace

void Game::camp_shop() {
    const int kItemCount = 12;
    const int kWeaponCount = 10;
    const int kSellItem = kItemCount + kWeaponCount + 1;
    // Sell-weapon is the last row (kSellItem + 1); the trailing else covers it
    // since read_choice already bounds input to the printed count.
    while (true) {
        int level = player->get_level();
        cout << "____________________________________________" << endl;
        cout << "Shop (your gold: " << player->get_gold() << ", level: " << level << ")" << endl;
        int n = 0;
        cout << "--- Potions ---" << endl;
        for (int i = 0; i < kItemCount; i++) {
            Item tmp(kShopItems[i]);
            int price = shop_item_price(tmp.getType());
            int unlock = shop_item_unlock(tmp.getType());
            cout << ++n << ". Buy " << tmp.get_name() << " (" << price << "g)";
            if (level < unlock) cout << " [Unlocks at Lv " << unlock << "]";
            cout << " -- " << tmp.get_effect_text() << endl;
        }
        cout << "--- Weapons ---" << endl;
        for (int i = 0; i < kWeaponCount; i++) {
            Weapon tmp(kShopWeapons[i]);
            int price = shop_weapon_price(tmp.getType());
            int unlock = shop_weapon_unlock(tmp.getType());
            cout << ++n << ". Buy " << tmp.get_name() << " (" << price << "g)";
            if (level < unlock) cout << " [Unlocks at Lv " << unlock << "]";
            cout << " -- " << tmp.get_effect_text() << endl;
        }
        cout << ++n << ". Sell an item (1/3 buy price)" << endl;
        cout << ++n << ". Sell a weapon (1/3 buy price)" << endl;
        cout << "0. Leave shop" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(0, n);
        if (choice == 0) {
            return;
        }
        if (choice <= kItemCount + kWeaponCount) {
            if (choice <= kItemCount) {
                Item::ItemType itype = kShopItems[choice - 1];
                int unlock = shop_item_unlock(itype);
                if (level < unlock) {
                    cout << "That stock unlocks at level " << unlock
                         << " (you are level " << level << ")." << endl;
                    continue;
                }
                int price = shop_item_price(itype);
                if (player->get_gold() < price) {
                    cout << "Not enough gold." << endl;
                    continue;
                }
                player->lose_gold(price);
                bag.addItem(Item(itype));
                cout << "Bought " << Item(itype).get_name() << "." << endl;
            } else {
                Weapon::WeaponType wtype = kShopWeapons[choice - kItemCount - 1];
                int unlock = shop_weapon_unlock(wtype);
                if (level < unlock) {
                    cout << "That stock unlocks at level " << unlock
                         << " (you are level " << level << ")." << endl;
                    continue;
                }
                int price = shop_weapon_price(wtype);
                if (player->get_gold() < price) {
                    cout << "Not enough gold." << endl;
                    continue;
                }
                player->lose_gold(price);
                bag.addWeapon(Weapon(wtype));
                cout << "Bought a " << Weapon(wtype).get_name() << ". Equip it from the camp menu." << endl;
            }
        } else if (choice == kSellItem) {
            const vector<Item>& items = bag.getItems();
            if (items.empty()) {
                cout << "No items to sell." << endl;
                continue;
            }
            for (size_t i = 0; i < items.size(); i++) {
                cout << i + 1 << ". " << items[i].get_name()
                     << " (sell " << shop_item_sell(items[i].getType()) << "g)"
                     << " -- " << items[i].get_effect_text() << endl;
            }
            cout << "0. Back" << endl;
            cout << "Enter your choice: ";
            int pick = read_choice(0, static_cast<int>(items.size()));
            if (pick == 0) {
                continue;
            }
            Item sold = items[static_cast<size_t>(pick) - 1];
            int value = shop_item_sell(sold.getType());
            bag.removeItem(sold);
            player->gain_gold(value);
            cout << "Sold " << sold.get_name() << " for " << value << "g." << endl;
        } else {
            const vector<Weapon>& weapons = bag.getWeapons();
            if (weapons.empty()) {
                cout << "No weapons to sell." << endl;
                continue;
            }
            for (size_t i = 0; i < weapons.size(); i++) {
                cout << i + 1 << ". " << weapons[i].get_name()
                     << " (sell " << shop_weapon_sell(weapons[i].getType()) << "g)"
                     << " -- " << weapons[i].get_effect_text() << endl;
            }
            cout << "0. Back" << endl;
            cout << "Enter your choice: ";
            int pick = read_choice(0, static_cast<int>(weapons.size()));
            if (pick == 0) {
                continue;
            }
            Weapon sold = weapons[static_cast<size_t>(pick) - 1];
            int value = shop_weapon_sell(sold.getType());
            bag.removeWeapon(sold);
            player->gain_gold(value);
            cout << "Sold " << sold.get_name() << " for " << value << "g." << endl;
        }
    }
}

// ---- the wave loop ----

Game::Result Game::run() {
    if (!has_game()) {
        throw logic_error("Game::run called before new_game or load_state");
    }
    while (true) {
        // After pausing in the middle of a wave we go straight back into the fight instead of the camp
        if (!combat->is_wave_active()) {
            if (!camp()) {
                return Result::Paused;
            }
        }
        Combat::Result result = combat->fight(wave, difficulty);
        if (result == Combat::Result::Paused) {
            return Result::Paused;
        }
        if (result == Combat::Result::PlayerDied) {
            cout << "____________________________________________" << endl;
            cout << "GAME OVER - you reached wave " << wave << " as a level " << player->get_level()
                 << " " << player->Type_to_string() << " with " << player->get_gold() << " gold." << endl;
            cout << "____________________________________________" << endl;
            combat.reset();
            player.reset();
            return Result::GameOver;
        }
        wave++; // wave cleared: back to camp for the next one
    }
}

// ---- settings and read access ----

int Game::get_difficulty() const {
    return difficulty;
}
string Game::get_difficulty_name() const {
    switch (difficulty) {
    case 1: return "Easy";
    case 3: return "Hard";
    default: return "Normal";
    }
}
void Game::set_difficulty(int d) {
    if (d < 1) d = 1;
    if (d > 3) d = 3;
    difficulty = d;
}
int Game::get_wave() const {
    return wave;
}
bool Game::is_mid_wave() const {
    return combat.has_value() && combat->is_wave_active();
}
const Player& Game::get_player() const {
    return *player;
}
const Bag& Game::get_bag() const {
    return bag;
}