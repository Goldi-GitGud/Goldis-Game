#pragma once
#include <string>

class Player; // Forward declaration of Player class (Player.h includes Item.h, so we can't include it back)

class Item {
public:
    enum class ItemType {
        Small_Health_potion,
        Small_Defense_potion,
        Small_Dodge_potion,
        Small_Attack_potion,
        Medium_Health_potion,
        Medium_Defense_potion,
        Medium_Dodge_potion,
        Medium_Attack_potion,
        Large_Health_potion,
        Large_Defense_potion,
        Large_Dodge_potion,
        Large_Attack_potion,
    };
private:
    ItemType type;
    int health_bonus;
    int attack_bonus;
    int armor_bonus;
    int dodge_bonus;
public:
    explicit Item(ItemType it);
    ItemType getType() const;
    int getHealthBonus() const;
    int getAttackBonus() const;
    int getArmorBonus() const;
    int getDodgeBonus() const;
    std::string Type_to_string(ItemType it) const;
    std::string get_name() const;
    // Health heals right away. Attack/armor/dodge go into the player's bonus stats
    // and last until Player::clear_temporary_effects() (called when a wave ends).
    void apply_effect(Player& player) const;
    void setHealthBonus(int bonus);
    void setAttackBonus(int bonus);
    void setArmorBonus(int bonus);
    void setDodgeBonus(int bonus);
};