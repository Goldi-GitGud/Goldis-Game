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
    auto it = find_if(items.begin(), items.end(),
        [&item](const Item& other) { return other.getType() == item.getType(); });
    if (it != items.end()) {
        items.erase(it);
    }
}
void Bag::removeWeapon(const Weapon& weapon) {
    auto it = find_if(weapons.begin(), weapons.end(),
        [&weapon](const Weapon& other) { return other.getType() == weapon.getType(); });
    if (it != weapons.end()) {
        weapons.erase(it);
    }
}
const vector<Item>& Bag::getItems() const {
    return items;
}
const vector<Weapon>& Bag::getWeapons() const {
    return weapons;
}
void Bag::displayContents() const {
    cout << "Items in Bag:" << endl;
    for (const auto& item : items) {
        cout << item.Type_to_string(item.getType()) << " ";
    }
    cout << endl;
    cout << "Weapons in Bag:" << endl;
    for (const auto& weapon : weapons) {
        cout << weapon.Type_to_string(weapon.getType()) << " ";
    }
    cout << endl;
}