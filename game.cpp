#include <iostream>
#include "game.h"

Game::Game(int size, Player* player1, Player* player2)
    : board(size), current_player_idx(0) {
    players[0] = player1;
    players[1] = player2;
}

void Game::switch_player() {
    current_player_idx = 1 - current_player_idx;
}

void Game::play() {
    char winner = '.';
    while (winner == '.') {
        board.display_board(); 
        char current_player = current_player_idx == 0 ? 'B' : 'R';
        std::pair<int, int> move = players[current_player_idx]->choose_move(board, current_player);
        // Output the chosen move
        int row = move.first + 1;
        char col = move.second + 'a';
        std::cout << "Player " << current_player << " chose move: " << row << " " << col << std::endl;
        board.make_move(move.first, move.second, current_player);
        winner = board.check_winner();
        switch_player();
    }
    board.display_board();
    char winning_player = (current_player_idx == 0) ? 'R' : 'B';
    std::cout << "Player " << winning_player << " wins!" << std::endl;
}

