#pragma once
#include <string>

class Weapon {
public:
    enum class WeaponType {
        Sword,
        Greatsword,
        Dagger,
        Spear,
        Axe,
        Mace,
        Greataxe,
        Katana,
        Twinblade,
        Saber
    };
private:
    WeaponType type;
    int health_bonus;
    int attack_bonus;
    int armor_bonus;
    int dodge_bonus;
public:
    explicit Weapon(WeaponType wt);
    WeaponType getType() const;
    int getHealthBonus() const;
    int getAttackBonus() const;
    int getArmorBonus() const;
    int getDodgeBonus() const;
    std::string Type_to_string(WeaponType wt) const;
    static WeaponType string_to_type(const std::string& name);
    std::string get_name() const;
    void display_stats() const;
    void setHealthBonus(int bonus);
    void setAttackBonus(int bonus);
    void setArmorBonus(int bonus);
    void setDodgeBonus(int bonus);
};