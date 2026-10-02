#include<iostream>
using namespace std;

class Game {

}

class Manager {

}


int main()
{
    Menu menu;
    Player player;
    Enemy enemy;// enemy classes will be a json file
    Weapon weapon;
    Item item;
    Bag bag;//for carrying items/weapons
    // Player_class player_class; this will be a json file
    Combat combat;
    Game game;
    Manager manager;
    menu.display_menu();
    return 0;
}