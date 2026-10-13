#include "Bag.h"
#include <algorithm>
#include <iostream>
using namespace std;

// Bag() = default is already written inside Bag.h, so it must not be defined again here.

void Bag::addItem(const Item& item) {
    items.push_back(item);
}
void Bag::addWeapon(const Weapon& weapon) {
    weapons.push_back(weapon);
}
// Items and Weapons have no operator==, so find() can't compare them directly.
// find_if lets us say what "the same" means: the same type. Removes the first match only.
void Bag::removeItem(const Item& item) {
    removeItemByType(item.getType());
}
void Bag::removeWeapon(const Weapon& weapon) {
    removeWeaponByType(weapon.getType());
}
const vector<Item>& Bag::getItems() const {
    return items;
}
const vector<Weapon>& Bag::getWeapons() const {
    return weapons;
}
void Bag::displayContents() const {
    cout << "--- Bag ---" << endl;
    cout << "Items (" << items.size() << "):" << endl;
    if (items.empty()) {
        cout << "  (empty)" << endl;
    } else {
        // Group by type, keeping first-appearance order; the effect preview
        // comes from the stored instance so tweaked bonuses still show.
        vector<Item::ItemType> order;
        for (const auto& item : items) {
            if (find(order.begin(), order.end(), item.getType()) == order.end())
                order.push_back(item.getType());
        }
        int line = 1;
        for (Item::ItemType type : order) {
            size_t n = countItem(type);
            const Item* first = nullptr;
            for (const auto& item : items) {
                if (item.getType() == type) { first = &item; break; }
            }
            cout << "  " << line++ << ". " << first->get_name()
                 << " x" << n << " -- " << first->get_effect_text() << endl;
        }
    }
    cout << "Weapons (" << weapons.size() << "):" << endl;
    if (weapons.empty()) {
        cout << "  (empty)" << endl;
    } else {
        vector<Weapon::WeaponType> order;
        for (const auto& weapon : weapons) {
            if (find(order.begin(), order.end(), weapon.getType()) == order.end())
                order.push_back(weapon.getType());
        }
        int line = 1;
        for (Weapon::WeaponType type : order) {
            size_t n = countWeapon(type);
            const Weapon* first = nullptr;
            for (const auto& weapon : weapons) {
                if (weapon.getType() == type) { first = &weapon; break; }
            }
            cout << "  " << line++ << ". " << first->get_name()
                 << " x" << n << " -- " << first->get_effect_text() << endl;
        }
    }
}

size_t Bag::itemCount() const {
    return items.size();
}
size_t Bag::weaponCount() const {
    return weapons.size();
}
bool Bag::empty() const {
    return items.empty() && weapons.empty();
}
size_t Bag::countItem(Item::ItemType type) const {
    return count_if(items.begin(), items.end(),
        [type](const Item& item) { return item.getType() == type; });
}
size_t Bag::countWeapon(Weapon::WeaponType type) const {
    return count_if(weapons.begin(), weapons.end(),
        [type](const Weapon& weapon) { return weapon.getType() == type; });
}
bool Bag::hasItem(Item::ItemType type) const {
    return countItem(type) > 0;
}
bool Bag::hasWeapon(Weapon::WeaponType type) const {
    return countWeapon(type) > 0;
}
bool Bag::removeItemByType(Item::ItemType type) {
    auto it = find_if(items.begin(), items.end(),
        [type](const Item& other) { return other.getType() == type; });
    if (it == items.end()) return false;
    items.erase(it);
    return true;
}
bool Bag::removeWeaponByType(Weapon::WeaponType type) {
    auto it = find_if(weapons.begin(), weapons.end(),
        [type](const Weapon& other) { return other.getType() == type; });
    if (it == weapons.end()) return false;
    weapons.erase(it);
    return true;
}
bool Bag::takeWeapon(Weapon::WeaponType type, Weapon& out) {
    auto it = find_if(weapons.begin(), weapons.end(),
        [type](const Weapon& other) { return other.getType() == type; });
    if (it == weapons.end()) return false;
    out = *it;
    weapons.erase(it);
    return true;
}