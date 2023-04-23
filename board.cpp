#include <iostream>
#include <cctype>
#include <vector>
#include <stdexcept>
#include "board.h"

Board::Board(int size) : board_size(size), board(size, std::vector<char>(size, '.')) {}

//similar to the neurobenzene implementation
void Board::display_board() const {
    int i, j, k;
    for (i = 0; i < board_size; i++) {
        char c = 'a' + i;
        std::cout << ' ' << c << ' ';
    }
    std::cout << std::endl;
    for (i = 0; i <= board_size; i++) {
        if (i >= 1)
            std::cout << ' ';
        for (k = 0; k < i - 1; k++)
            std::cout << ' ';
        if (i == board_size) {
            std::cout << ' ' << ' ';
            for (j = 0; j < board_size; j++) {
                char c = 'a' + j;
                std::cout << c << ' ' << ' ';
            }
            std::cout << std::endl;
            continue;
        }
        std::cout << (i + 1) << "\\";
        for (j = 0; j < board_size; j++) {
            if (j <= board_size - 2) {
                char c = board[i][j];
                if (c >= 'a' && c <= 'z') c = toupper(c);
                std::cout << c << ' ' << ' ';
            }
            if (j == board_size - 1) {
                char c = board[i][j];
                if (c >= 'a' && c <= 'z') c = toupper(c);
                std::cout << c << '\\' << (i + 1);
            }
        }
        std::cout << std::endl;
    }
}

//just dots
//void Board::display_board() const {
//    for (int i = 0; i < board_size; ++i) {
//        for (int k = 0; k < i; ++k) {
//            std::cout << " ";
//        }
//        for (int j = 0; j < board_size; ++j) {
//            std::cout << board[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
//}



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

void Board::dfs(int x, int y, char player, std::vector<std::vector<bool>>& visited) const {
    static const std::vector<int> dx = { -1, 0, 1, 1, 0, -1 };
    static const std::vector<int> dy = { -1, -1, 0, 1, 1, 0 };

    visited[x][y] = true;

    for (int i = 0; i < 6; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (is_within_bounds(nx, ny) && !visited[nx][ny] && board[nx][ny] == player) {
            dfs(nx, ny, player, visited);
        }
    }
}

char Board::check_winner() const {
    char players[] = { 'H', 'V' };
    for (char player : players) {
        std::vector<std::vector<bool>> visited(board_size, std::vector<bool>(board_size, false));
        if (player == 'H') {
            for (int x = 0; x < board_size; ++x) {
                if (board[x][0] == 'H' && !visited[x][0]) {
                    dfs(x, 0, 'H', visited);
                }
            }

            for (int x = 0; x < board_size; ++x) {
                if (visited[x][board_size - 1]) {
                    return 'H';
                }
            }
        }
        else {
            for (int y = 0; y < board_size; ++y) {
                if (board[0][y] == 'V' && !visited[0][y]) {
                    dfs(0, y, 'V', visited);
                }
            }

            for (int y = 0; y < board_size; ++y) {
                if (visited[board_size - 1][y]) {
                    return 'V';
                }
            }
        }
    }

    return '.';
}

int Board::get_board_size() const {
    return board_size;
}