#include "board.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "iterator"

Board::Board(int size)
    : board_size(size),
      board(size, std::vector<Cell_state>(size, Cell_state::Empty)) {
  if (size < 2) {
    throw std::invalid_argument("Board size cannot be less than 2.");
  }
}

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
  board.display_board();
  return os;
}

bool Board::is_within_bounds(int move_x, int move_y) const {
  return move_x >= 0 && move_x < board_size && move_y >= 0 &&
         move_y < board_size;
}

bool Board::is_valid_move(int move_x, int move_y) const {
  return is_within_bounds(move_x, move_y) &&
         (board[move_x][move_y] == Cell_state::Empty);
}

void Board::make_move(int move_x, int move_y, Cell_state player) {
  if (!is_valid_move(move_x, move_y)) {
    throw std::invalid_argument("Invalid move attempt at position (" +
                                std::to_string(move_x) + ", " +
                                std::to_string(move_y) + ")!");
  }
  board[move_x][move_y] = player;
}

int Board::get_board_size() const { return board_size; }

bool Board::depth_first_search(
    int start_x, int start_y, int destination_x, int destination_y,
    Cell_state player_symbol,
    std::vector<std::vector<Cell_state>>& game_board_snapshot) const {
  if (start_x == destination_x && start_y == destination_y) return true;
  game_board_snapshot[start_x][start_y] = Cell_state::Empty;
  for (int i = 0; i < neighbour_offset_x.size(); ++i) {
    int new_x = start_x + neighbour_offset_x[i];
    int new_y = start_y + neighbour_offset_y[i];
    if (is_within_bounds(new_x, new_y) &&
        game_board_snapshot[new_x][new_y] == player_symbol &&
        depth_first_search(new_x, new_y, destination_x, destination_y,
                           player_symbol, game_board_snapshot)) {
      game_board_snapshot[start_x][start_y] = player_symbol;
      return true;
    }
  }
  game_board_snapshot[start_x][start_y] = player_symbol;
  return false;
}

Cell_state Board::check_winner() const {
  // Check for player B (top to bottom)
  for (int top_row_index = 0; top_row_index < board_size; ++top_row_index) {
    if (board[0][top_row_index] == Cell_state::Blue) {
      for (int bottom_row_index = 0; bottom_row_index < board_size;
           ++bottom_row_index) {
        std::vector<std::vector<Cell_state>> board_copy = board;
        if (board[board_size - 1][bottom_row_index] == Cell_state::Blue &&
            depth_first_search(0, top_row_index, board_size - 1,
                               bottom_row_index, Cell_state::Blue,
                               board_copy)) {
          return Cell_state::Blue;
        }
      }
    }
  }
  // Check for player R (left to right)
  for (int left_column_index = 0; left_column_index < board_size;
       ++left_column_index) {
    if (board[left_column_index][0] == Cell_state::Red) {
      for (int right_column_index = 0; right_column_index < board_size;
           ++right_column_index) {
        std::vector<std::vector<Cell_state>> board_copy = board;
        if (board[right_column_index][board_size - 1] == Cell_state::Red &&
            depth_first_search(left_column_index, 0, right_column_index,
                               board_size - 1, Cell_state::Red, board_copy)) {
          return Cell_state::Red;
        }
      }
    }
  }
  return Cell_state::Empty;
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

// THE FOLLOWING METHOD IS KEPT ONLY FOR DEBUGGING THE BOARD. THEY ARE NOT
// USED IN THE GAME.
bool Board::are_cells_connected(int first_cell_x, int first_cell_y,
                                int second_cell_x, int second_cell_y) const {
  for (int i = 0; i < neighbour_offset_x.size(); ++i) {
    int neighbour_cell_x = first_cell_x + neighbour_offset_x[i];
    int neighbour_cell_y = first_cell_y + neighbour_offset_y[i];
    if (is_within_bounds(neighbour_cell_x, neighbour_cell_y) &&
        neighbour_cell_x == second_cell_x &&
        neighbour_cell_y == second_cell_y) {
      return true;
    }
  }
  return false;
}



