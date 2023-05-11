#include <iostream>
#include <cctype>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "iterator"
#include "hex_board.h"

Board::Board(int size) : board_size(size), board(size, std::vector<char>(size, '.')) {}


void Board::display_board() const {
    std::cout << "\n";
    for (size_t i = 0; i < board_size; ++i) {
        std::cout << std::string(2 * i, ' ');
        for (size_t j = 0; j < board_size; ++j) {
            std::cout << board[i][j];
            if (j < board_size - 1) {
                std::cout << " - ";
            }
        }
        std::cout << " " << i + 1;
        std::cout << "\n";

        if (i < board_size - 1) {
            std::cout << std::string(2 * i + 1, ' ');
            for (size_t j = 0; j < board_size - 1; ++j) {
                std::cout << "\\ / ";
            }
            std::cout << "\\";
            std::cout << "\n";
        }
    }
    // print the bottom coordinate labels
    std::cout << std::string(2 * (board_size - 1) - 1, ' ');
    for (size_t j = 0; j < board_size; ++j) {
        std::cout << "  " << static_cast<char>('a' + j) << " ";
    }
    std::cout << "\n\n";
}

//neurobenzene
//void Board::display_board() const {
//    for (int i = 0; i < board_size; i++) {
//        char c = 'a' + i;
//        std::cout << ' ' << c << ' ';
//    }
//    std::cout << std::endl;
//    for (int i = 0; i <= board_size; i++) {
//        if (i >= 1)
//            std::cout << ' ';
//        for (int k = 0; k < i - 1; k++)
//            std::cout << ' ';
//        if (i == board_size) {
//            std::cout << ' ' << ' ';
//            for (int j = 0; j < board_size; j++) {
//                char c = 'a' + j;
//                std::cout << c << ' ' << ' ';
//            }
//            std::cout << std::endl;
//            continue;
//        }
//        std::cout << (i + 1) << "\\";
//        for (int j = 0; j < board_size; j++) {
//            if (j <= board_size - 2) {
//                char c = board[i][j];
//                if (c >= 'a' && c <= 'z') c = toupper(c);
//                std::cout << c << ' ' << ' ';
//            }
//            if (j == board_size - 1) {
//                char c = board[i][j];
//                if (c >= 'a' && c <= 'z') c = toupper(c);
//                std::cout << c << '\\' << (i + 1);
//            }
//        }
//        std::cout << std::endl;
//    }
//}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    // Use the display_board function to print the board state
    board.display_board();
    return os;
}

bool Board::is_valid_move(int x, int y) const {
    return (x >= 0 && x < board_size) && (y >= 0 && y < board_size) && (board[x][y] == '.');
}

void Board::make_move(int x, int y, char player) {
    if (is_valid_move(x, y)) {
        board[x][y] = player;
    }
    else {
        throw std::invalid_argument("Invalid move!");
    }
}

bool Board::is_within_bounds(int x, int y) const {
    return x >= 0 && x < board_size&& y >= 0 && y < board_size;
}

int Board::get_board_size() const {
    return board_size;
}

bool Board::dfs(int x, int y, int target_x, int target_y, char player, std::vector<std::vector<char>>& temp_board) const{
    if (x == target_x && y == target_y) return true;

    temp_board[x][y] = '.';

    for (int i = 0; i < 6; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (is_within_bounds(nx, ny) && temp_board[nx][ny] == player && dfs(nx, ny, target_x, target_y, player, temp_board)) {
            temp_board[x][y] = player;
            return true;
        }
    }

    temp_board[x][y] = player;
    return false;
}

bool Board::is_connected(int x1, int y1, int x2, int y2) {
    for (int i = 0; i < 6; ++i) {
        int nx = x1 + dx[i];
        int ny = y1 + dy[i];

        if (is_within_bounds(nx, ny) && nx == x2 && ny == y2) {
            return true;
        }
    }
    return false;
}

char Board::check_winner() const{
    // Check for player B (top to bottom)
    for (int i = 0; i < board_size; ++i) {
        if (board[0][i] == 'B') {
            for (int j = 0; j < board_size; ++j) {
                std::vector<std::vector<char>> temp_board = board;
                if (board[board_size - 1][j] == 'B' && dfs(0, i, board_size - 1, j, 'B', temp_board)) {
                    return 'B';
                }
            }
        }
    }

    // Check for player R (left to right)
    for (int i = 0; i < board_size; ++i) {
        if (board[i][0] == 'R') {
            for (int j = 0; j < board_size; ++j) {
                std::vector<std::vector<char>> temp_board = board;
                if (board[j][board_size - 1] == 'R' && dfs(i, 0, j, board_size - 1, 'R', temp_board)) {
                    return 'R';
                }
            }
        }
    }
    return '.';
}


