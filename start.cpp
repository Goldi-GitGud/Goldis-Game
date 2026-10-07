#include <iostream>
#include <exception>
#include "Menu.h"
using namespace std;

int main()
{
    try {
        Menu menu; // the menu owns the Game and the Manager, and the Game owns the Player, Bag and Combat
        menu.display_menu();
    }
    catch (const exception& e) {
        // Most likely a data file (Player_data.json, Enemy_data.json ...) was not found next to where the program runs
        cout << "The game stopped because of an error: " << e.what() << endl;
        return 1;
    }
    return 0;
}