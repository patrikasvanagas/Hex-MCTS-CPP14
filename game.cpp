#include <iostream>
#include "game.h"

Game::Game(int board_size , Player* player1, Player* player_2)
    : board(board_size ), current_player_index(0) {
    players[0] = player1;
    players[1] = player_2;
}

void Game::switch_player() {
    current_player_index = 1 - current_player_index;
}

void Game::play() {
    while (board.check_winner() == '.') {
        board.display_board(); 
        char current_player = current_player_index == 0 ? 'B' : 'R';
        std::pair<int, int> chosen_move = players[current_player_index]->choose_move(board, current_player);
        int chosen_row  = chosen_move.first + 1;
        char chosen_col = chosen_move.second + 'a';
        std::cout << "Player " << current_player << " chose move: " << chosen_row  << " " << chosen_col << std::endl;
        board.make_move(chosen_move.first, chosen_move.second, current_player);
        switch_player();
    }
    board.display_board();
    char winning_player = (current_player_index == 0) ? 'R' : 'B';
    std::cout << "Player " << winning_player << " wins!" << std::endl;
}

