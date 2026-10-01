#pragma once

class Menu {
public:
    void display_menu();
    void launch_game();
    void launch_pause();
    void launch_options();
    void launch_exit(int exit_code);
    void difficulty(int loc);
}
