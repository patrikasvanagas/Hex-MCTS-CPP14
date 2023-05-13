#include <iostream>
#include <cctype>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <string>
#include "iterator"
#include "board.h"

Board::Board(int size) : board_size(size), board(size, std::vector<char>(size, '.')) {}

void Board::display_board() const {
    std::cout << "\n";
    for (size_t row = 0; row < board_size; ++row) {
        std::cout << std::string(2 * row, ' ');
        for (size_t col = 0; col < board_size; ++col) {
            std::cout << board[row][col];
            if (col < board_size - 1) {
                std::cout << " - ";
            }
        }
        std::cout << " " << row + 1;
        std::cout << "\n";

        if (row < board_size - 1) {
            std::cout << std::string(2 * row + 1, ' ');
            for (size_t col = 0; col < board_size - 1; ++col) {
                std::cout << "\\ / ";
            }
            std::cout << "\\";
            std::cout << "\n";
        }
    }
    // print the bottom coordinate labels
    std::cout << std::string(2 * (board_size - 1) - 1, ' ');
    for (size_t col = 0; col < board_size; ++col) {
        std::cout << "  " << static_cast<char>('a' + col) << " ";
    }
    std::cout << "\n\n";
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    // Use the display_board function to print the board state
    board.display_board();
    return os;
}

bool Board::is_valid_move(int move_x, int move_y) const {
    return (move_x >= 0 && move_x < board_size) && (move_y >= 0 && move_y < board_size) && (board[move_x][move_y] == '.');
}

void Board::make_move(int move_x, int move_y, char player) {
    if (!is_valid_move(move_x, move_y)) {
        throw std::invalid_argument("Invalid move attempt at position (" + std::to_string(move_x) + ", " + std::to_string(move_y) + ")!");
    }
    board[move_x][move_y] = player;
}

bool Board::is_within_bounds(int move_x, int move_y) const {
    return move_x >= 0 && move_x < board_size&& move_y >= 0 && move_y < board_size;
}

int Board::get_board_size() const {
    return board_size;
}

bool Board::depth_first_search(int start_x, int start_y, int destination_x, int destination_y, char player_symbol, std::vector<std::vector<char>>& game_board_snapshot) const {
    if (start_x == destination_x && start_y == destination_y) return true;
    game_board_snapshot[start_x][start_y] = '.';
    for (int i = 0; i < neighbour_offset_x.size(); ++i) {
        int new_x = start_x + neighbour_offset_x[i];
        int new_y = start_y + neighbour_offset_y[i];
        if (is_within_bounds(new_x, new_y) && game_board_snapshot[new_x][new_y] == player_symbol && depth_first_search(new_x, new_y, destination_x, destination_y, player_symbol, game_board_snapshot)) {
            game_board_snapshot[start_x][start_y] = player_symbol;
            return true;
        }
    }
    game_board_snapshot[start_x][start_y] = player_symbol;
    return false;
}



char Board::check_winner() const {
    // Check for player B (top to bottom)
    for (int top_row_index = 0; top_row_index < board_size; ++top_row_index) {
        if (board[0][top_row_index] == 'B') {
            for (int bottom_row_index = 0; bottom_row_index < board_size; ++bottom_row_index) {
                std::vector<std::vector<char>> board_copy = board;
                if (board[board_size - 1][bottom_row_index] == 'B' && depth_first_search(0, top_row_index, board_size - 1, bottom_row_index, 'B', board_copy)) {
                    return 'B';
                }
            }
        }
    }
    // Check for player R (left to right)
    for (int left_column_index = 0; left_column_index < board_size; ++left_column_index) {
        if (board[left_column_index][0] == 'R') {
            for (int right_column_index = 0; right_column_index < board_size; ++right_column_index) {
                std::vector<std::vector<char>> board_copy = board;
                if (board[right_column_index][board_size - 1] == 'R' && depth_first_search(left_column_index, 0, right_column_index, board_size - 1, 'R', board_copy)) {
                    return 'R';
                }
            }
        }
    }
    return '.';
}


std::vector<std::pair<int, int>> Board::get_valid_moves() const {
    std::vector<std::pair<int, int>> valid_moves;
    for (int row = 0; row < board_size; ++row) {
        for (int col = 0; col < board_size; ++col) {
            if (is_valid_move(row, col)) {
                valid_moves.emplace_back(row, col);
            }
        }
    }
    return valid_moves;
}

//kept only for debugging the board class.
bool Board::are_cells_connected(int first_cell_x, int first_cell_y, int second_cell_x, int second_cell_y) {
    for (int i = 0; i < neighbour_offset_x.size(); ++i) {
        int neighbour_cell_x = first_cell_x + neighbour_offset_x[i];
        int neighbour_cell_y = first_cell_y + neighbour_offset_y[i];
        if (is_within_bounds(neighbour_cell_x, neighbour_cell_y) && neighbour_cell_x == second_cell_x && neighbour_cell_y == second_cell_y) {
            return true;
        }
    }
    return false;
}
