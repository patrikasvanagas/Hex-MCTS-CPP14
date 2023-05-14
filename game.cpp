#include <iostream>
#include "game.h"

Game::Game(int board_size, std::unique_ptr<Player> player1, std::unique_ptr<Player> player_2)
    : board(board_size), current_player_index(0) {
    players[0] = std::move(player1);
    players[1] = std::move(player_2); 
}

void Game::switch_player() {
    current_player_index = 1 - current_player_index;
}

void Game::play() {
    while (board.check_winner() == Cell_state::Empty) {
        Cell_state current_player = current_player_index == 0 ? Cell_state::Blue : Cell_state::Red;
        std::cout << "\nPlayer " << current_player << "'s turn:" << std::endl;
        board.display_board();
        std::pair<int, int> chosen_move = players[current_player_index]->choose_move(board, current_player);
        int chosen_row = chosen_move.first + 1;
        char chosen_col = chosen_move.second + 'a';
        std::cout << "\nPlayer " << current_player << " chose move: " << chosen_row << " " << chosen_col << std::endl;
        board.make_move(chosen_move.first, chosen_move.second, current_player);
        switch_player();
    }
    board.display_board();
    Cell_state winning_player = (current_player_index == 0) ? Cell_state::Red : Cell_state::Blue;
    std::cout << "Player " << winning_player << " wins!" << std::endl;
}
