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
    return std::make_pair(-1, -1);
}

MCTSPlayer::MCTSPlayer(double exploration_factor, std::chrono::milliseconds max_decision_time, bool is_parallelized, bool is_verbose)
    : exploration_factor(exploration_factor), max_decision_time(max_decision_time), is_parallelized(is_parallelized), is_verbose(is_verbose) {}

std::pair<int, int> MCTSPlayer::choose_move(const Board& board, char player) {
    Mcts_agent agent(exploration_factor, max_decision_time, is_parallelized, is_verbose);
    return agent.choose_move(board, player);
}
