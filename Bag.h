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
    // Read-only views of what's inside, so Combat (use item) and Manager (saving) can look without being able to change it.
    const std::vector<Item>& getItems() const;
    const std::vector<Weapon>& getWeapons() const;
    void displayContents() const;
};