#include "Bag.h"
#include <iostream>
using namespace std;

Bag::Bag() = default;
void Bag::addItem(const Item& item) {
    items.push_back(item);
}
void Bag::addWeapon(const Weapon& weapon) {
    weapons.push_back(weapon);
}
void Bag::removeItem(const Item& item) {
    auto it = find(items.begin(), items.end(), item);
    if (it != items.end()) {
        items.erase(it);
    }
}
void Bag::removeWeapon(const Weapon& weapon) {
    auto it = find(weapons.begin(), weapons.end(), weapon);
    if (it != weapons.end()) {
        weapons.erase(it);
    }
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