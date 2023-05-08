#include <iostream>
#include <chrono>
#include "player.h"
#include "mcts_agent.h"

std::pair<int, int> HumanPlayer::choose_move(const Board& board, char player) {
    int x;
    char col_char;
    bool valid_move = false;

    while (!valid_move) {
        std::cout << "Enter the row as a number and the column as a letter separated by space: ";
        std::cin >> x >> col_char;

        if (col_char >= 'a' && col_char < 'a' + board.get_board_size()) {
            int y = col_char - 'a';
            if (board.is_valid_move(x - 1, y)) {
                return std::make_pair(x - 1, y);
            }
            else {
                std::cout << "Invalid move! Try again." << std::endl;
            }
        }
        else {
            std::cout << "Invalid column! Try again." << std::endl;
        }
    }
}

MCTSPlayer::MCTSPlayer(double exploration_constant, std::chrono::milliseconds move_time_limit)
    : exploration_constant(exploration_constant), move_time_limit(move_time_limit) {}

std::pair<int, int> MCTSPlayer::choose_move(const Board& board, char player) {
    MCTSAgent agent(exploration_constant, move_time_limit);
    return agent.choose_move(board, player);
}
