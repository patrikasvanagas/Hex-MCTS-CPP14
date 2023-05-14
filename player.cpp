#include <iostream>
#include <chrono>
#include "player.h"
#include "mcts_agent.h"

std::pair<int, int> Human_player::choose_move(const Board& board, Cell_state player) {
    int move_row;
    char move_col_symbol;
    bool valid_move = false;

    while (!valid_move) {
        std::cout << "Enter the row as a number and the column as a letter separated by space: ";
        if (!(std::cin >> move_row >> move_col_symbol)) {
            std::cout << "Invalid input! Try again." << std::endl;
            std::cin.clear();  // clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ignore the rest of the line
            continue;
        }
        if (move_row < 1 || move_row > board.get_board_size()) {
            std::cout << "Invalid row! Try again." << std::endl;
            continue;
        }
        if (move_col_symbol >= 'a' && move_col_symbol < 'a' + board.get_board_size()) {
            int move_col = move_col_symbol - 'a';
            if (board.is_valid_move(move_row - 1, move_col)) {
                return std::make_pair(move_row - 1, move_col);
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

Mcts_player::Mcts_player(double exploration_factor, std::chrono::milliseconds max_decision_time, bool is_parallelized, bool is_verbose)
    : exploration_factor(exploration_factor), max_decision_time(max_decision_time), is_parallelized(is_parallelized), is_verbose(is_verbose) {}

std::pair<int, int> Mcts_player::choose_move(const Board& board, Cell_state player) {
    Mcts_agent agent(exploration_factor, max_decision_time, is_parallelized, is_verbose);
    return agent.choose_move(board, player);
}