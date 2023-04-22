#include <iostream>
#include "game.h"
#include "mcts_agent.h"

Game::Game(int size) : board(size), current_player('B') {}

void Game::switch_player() {
    current_player = (current_player == 'B') ? 'W' : 'B';
}

void Game::make_agent_move() {
    MCTSAgent agent(1000);  // Choose the number of simulations
    std::pair<int, int> move = agent.choose_move(board, current_player);
    board.make_move(move.first, move.second, current_player);
}

void Game::make_human_move() {
    int x, y;
    bool valid_move = false;

    while (!valid_move) {
        std::cout << "Enter the row and column separated by space: ";
        std::cin >> x >> y;

        try {
            board.make_move(x, y, current_player);
            valid_move = true;
        }
        catch (const std::invalid_argument&) {
            std::cout << "Invalid move! Try again." << std::endl;
        }
    }
}


void Game::play_game() {
    char winner = '.';
    while (winner == '.') {
        board.display_board();

        if (current_player == 'B') {
            make_human_move();
        }
        else {
            make_agent_move();
        }
        winner = board.check_winner();
        switch_player();
    }

    board.display_board();
    std::cout << "Player " << winner << " wins!" << std::endl;
}
