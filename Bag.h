#pragma once
#include <cstddef>
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
    // --- Equip/swap backend helpers (additive; existing API above is unchanged).
    // Counts of everything, for HUD/"bag full" checks.
    std::size_t itemCount() const;
    std::size_t weaponCount() const;
    bool empty() const;
    // Per-type queries, for equip/swap menus ("do I own a Katana?").
    std::size_t countItem(Item::ItemType type) const;
    std::size_t countWeapon(Weapon::WeaponType type) const;
    bool hasItem(Item::ItemType type) const;
    bool hasWeapon(Weapon::WeaponType type) const;
    // Remove one entry of the given type. Return true if one was removed.
    bool removeItemByType(Item::ItemType type);
    bool removeWeaponByType(Weapon::WeaponType type);
    // Remove one weapon of the given type and copy it to `out`, for
    // equip/swap flows (take new weapon out, put the old one back with
    // addWeapon). Returns true if one was taken.
    bool takeWeapon(Weapon::WeaponType type, Weapon& out);
    // Grouped, counted display with per-type effect preview.
    void displayContents() const;
};
