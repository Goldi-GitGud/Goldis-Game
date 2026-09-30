#include<iostream>
using namespace std;

class Game {

}

class Manager {

}

class Menu {
public:
    void display_menu() {
        while (true) {
            cout << "Welcome to Goldi's Game!" << endl;
            cout << "___________________________________________" << endl;
            cout << "1. Start Game" << endl;
            cout << "2. Options" << endl;
            cout << "3.Exit" << endl;
            cout << "___________________________________________" << endl;
            cout << "Enter your choice: ";
            int choice;
            cin >> choice;
            switch (choice)
            {
            case 1:
                launch_game();
                break;
            case 2:
                launch_options();
                break;
            case 3:
                launch_exit(0);
                break;
            default:
                launch_exit(1);
                break;
            }
        }
    }

    void launch_game() {
        while (true) {
            cout << "Welcome to the Game Menu!" << endl;
            cout << "____________________________________________" << endl;
            cout << "1. New Game" << endl;
            cout << "2. Load Game" << endl;
            cout << "3. Return to Main Menu" << endl;
            cout << "___________________________________________" << endl;
            cout << "Enter your choice: ";
            int choice;cin >> choice;
            switch (choice)
            {
            case 1:
                difficulty(2);
                break;
            case 2:
                manager.load_game_saves();
                break;
            case 3:
                return;
                break;
            default:
                cout << "Invalid choice. Returning to main menu." << endl;
                return;
                break;
            }
        }
    }
    void launch_pause() {
        while (true) {
            cout << "____________________________________________" << endl;
            cout << "1. Resume Game" << endl;
            cout << "2. Save Game" << endl;
            cout << "3. Change Difficulty" << endl;
            cout << "4. Return to Main Menu" << endl;
            cout << "5. Exit Game" << endl;
            cout << "___________________________________________" << endl;
            cout << "Enter your choice: ";
            int choice;cin >> choice;
            switch (choice)
            {
            case 1:
                game.resume_game();
                break;
            case 2:
                manager.save_game_saves();
                break;
            case 3:
                difficulty(4);
                break;
            case 4:
                cout << "Saving game before returning to main menu..." << endl;
                manager.save_game_saves();
                display_menu();
                break;
            case 5:
                cout << "Saving game before exiting..." << endl;
                manager.save_game_saves();
                launch_exit(0);
                break;
            default:
                cout << "Invalid choice. Resuming game." << endl;
                game.resume_game();
                break;
            }
        }
    }
    void launch_options() {
        while (true) {
            cout << "____________________________________________" << endl;
            cout << "1. Change Difficulty" << endl;
            cout << "2. Return to Main Menu" << endl;
            cout << "____________________________________________" << endl;
            cout << "Enter your choice: ";
            int choice;cin >> choice;
            switch (choice)
            {
            case 1:
                difficulty(1);
                break;
            case 2:
                return;
                break;
            default:
                cout << "Invalid choice. Returning to main menu." << endl;
                return;
                break;
            }
        }

    }
    void launch_exit(int code) {
        if (code == 0) {
            cout << "Thank you for playing the game!" << endl;
            return;
        }
        else {
            cout << "Invalid choice. Exiting the program." << endl;
            return;
        }
    }
    void difficulty(int loc) {
        while (true) {
            cout << "____________________________________________" << endl;
            cout << "Current difficulty level: " << game.get_difficulty() << endl;;
            cout << " choose difficulty level: " << endl;
            cout << "1. Easy (The enemies are weaker and you are a bit more stronger)" << endl;
            cout << "2. Normal (Balanced difficulty , recommended for most players)" << endl;
            cout << "3. Hard (The enemies are significantly stronger , recommended for experienced players)" << endl;
            cout << "4. Return to Previous Menu" << endl;
            cout << "Note: This will affect all saves and can be changed from the options menu." << endl;
            cout << "____________________________________________" << endl;
            cout << "Enter your choice: ";
            int choice;
            cin >> choice;
            switch (choice) {
            case 1:
                game.set_difficulty(1);
                cout << "Difficulty level set to Easy." << endl;
                if (loc == 2)loc = 3;
                break;
            case 2:
                game.set_difficulty(2);
                cout << "Difficulty level set to Normal." << endl;
                if (loc == 2)loc = 3;
                break;
            case 3:
                game.set_difficulty(3);
                cout << "Difficulty level set to Hard." << endl;
                if (loc == 2)loc = 3;
                break;
            default:
                cout << "Invalid choice." << endl;
                break;
            }
            if (loc == 1) {
                cout << "Returning to main menu" << endl;
                display_menu();
            }
            else if (loc == 2) {
                cout << "Returning to game menu" << endl;
                launch_game();
            }
            else if (loc == 3) {
                cout << "Starting Game..." << endl;
                game.start_game();
            }
            else {
                cout << "Returning to pause menu" << endl;
                launch_pause();
            }
        }
    }

}

int main()
{
    Manager manager;
    Game game;
    Menu menu;
    menu.display_menu();
    return 0;
}