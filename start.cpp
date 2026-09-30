#include<iostream>
using namespace std;

class Menu{
    public:
        void display_menu(){
            cout << "___________________________________________" << endl;
            cout << "1. Start Game" <<endl;
            cout << "2. Options" <<endl;
            cout << "3.Exit" <<endl;
            cout<< "___________________________________________" << endl;
            cout<< "Enter your choice: ";
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

        void launch_game(){

        }
        void launch_options(){
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
                display_menu();
                break;
            default:
                cout << "Invalid choice. Returning to main menu." << endl;
                display_menu();
                break;
            }

        }
        void launch_exit(int code){
            if(code == 0){
                cout << "Thank you for playing the game!" << endl;
                return;
            }
            else{
                cout << "Invalid choice. Exiting the program." << endl;
                return;
            }
        }
        void difficulty(int loc){
            cout << "____________________________________________" << endl;
            cout << "Current difficulty level: "<<game.get_difficulty()<<endl;;
            cout <<" choose difficulty level: " << endl;
            cout << "1. Easy (The enemies are weaker and you are a bit more stronger)" << endl;
            cout << "2. Normal (Balanced difficulty , recommended for most players)"<<endl;
            cout<<"3. Hard (The enemies are significantly stronger , recommended for experienced players)"<<endl;
            cout<< "4. Return to Previous Menu" << endl;
            cout<< "Note: This will affect all saves and can be changed from the options menu." << endl;
            cout << "____________________________________________" << endl;
            cout << "Enter your choice: ";
            int choice;
            cin >> choice;
            switch(choice){
                case 1:
                    game.set_difficulty(1);
                    cout << "Difficulty level set to Easy." << endl;
                    break;
                case 2:
                    game.set_difficulty(2);
                    cout << "Difficulty level set to Normal." << endl;
                    break;
                case 3:
                    game.set_difficulty(3);
                    cout << "Difficulty level set to Hard." << endl;
                    break;
                default:
                    cout << "Invalid choice. Returning to previous menu." << endl;
                    break;
            }
            if(loc == 1){
                cout<< "Returning to main menu." << endl;
                display_menu();
            }else if(loc == 2){
                cout<<"Returning to game menu"<<endl;
                launch_game();
            }else{
                cout<<"Returning to pause menu"<<endl;
                launch_pause();
            }
        }

}

int main()
{
    Game game;
    Menu menu;
    menu.display_menu();
    return 0;
}