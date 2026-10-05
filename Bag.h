#pragma once
#include <vector>
#include "Item.h"
#include "Weapon.h"

class Bag {
private:
    std::vector<Item> items;
    std::vector<Weapon> weapons;
public:
    Bag() = default;
    void addItem(const Item& item);
    void addWeapon(const Weapon& weapon);
    void removeItem(const Item& item);
    void removeWeapon(const Weapon& weapon);
    void displayContents() const;
};