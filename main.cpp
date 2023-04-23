#include <iostream>
#include <chrono>
#include "game.h"

int main() {

    //black connects top and bottom and goes first,
    // white connects left and right and goes second
    int board_size = 3;
    double exploration_constant = 2.0; // Adjust this value as needed
    std::chrono::milliseconds move_time_limit(300); // Adjust this value as needed

    // Let user choose their player color
    char human_player;
    std::cout << "Choose your player color (H/V): ";
    std::cin >> human_player;
    while (human_player != 'H' && human_player != 'V') {
        std::cout << "Invalid input! Choose your player color (H/V): ";
        std::cin >> human_player;
    }

    Game game(board_size, exploration_constant, move_time_limit, human_player);
    game.play_game();

    return 0;
}