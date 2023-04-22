#include <iostream>
#include "game.h"

int main() {
    int board_size;
    std::cout << "Enter the board size: ";
    std::cin >> board_size;

    Game game(board_size);
    game.play_game();

    return 0;
}
