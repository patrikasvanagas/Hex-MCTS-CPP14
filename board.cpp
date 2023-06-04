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

int Board::get_board_size() const { return board_size; }

bool Board::is_within_bounds(int move_x, int move_y) const {
  return move_x >= 0 && move_x < board_size && move_y >= 0 &&
         move_y < board_size;
}

bool Board::is_valid_move(int move_x, int move_y) const {
  return is_within_bounds(move_x, move_y) &&
         (board[move_x][move_y] == Cell_state::Empty);
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

void Board::make_move(int move_x, int move_y, Cell_state player) {
  // Check if the move is valid. If not, throw an exception.
  if (!is_valid_move(move_x, move_y)) {
    throw std::invalid_argument("Invalid move attempt at position (" +
                                std::to_string(move_x) + ", " +
                                std::to_string(move_y) + ")!");
  }
  // If the move is valid, place the player's Cell_state on the board at the
  // specified coordinates.
  board[move_x][move_y] = player;
}

bool Board::are_cells_connected(int first_cell_x, int first_cell_y,
                                int second_cell_x, int second_cell_y) const {
  // Iterate over all possible neighboring cells of the first cell
  for (int i = 0; i < neighbour_offset_x.size(); ++i) {
    int neighbour_cell_x = first_cell_x + neighbour_offset_x[i];
    int neighbour_cell_y = first_cell_y + neighbour_offset_y[i];
    if (is_within_bounds(neighbour_cell_x, neighbour_cell_y) &&
        neighbour_cell_x == second_cell_x &&
        neighbour_cell_y == second_cell_y) {
      return true;
    }
  }
  // If none of the neighboring cells match the second cell, return false.
  return false;
}

bool Board::depth_first_search(
    int start_x, int start_y, int destination_x, int destination_y,
    Cell_state player_symbol,
    std::vector<std::vector<Cell_state>>& game_board_snapshot) const {
  // Base case: if the current cell is the destination cell, a path has been
  // found.
  if (start_x == destination_x && start_y == destination_y) return true;
  // Mark the current cell as empty to prevent loops during the DFS.
  game_board_snapshot[start_x][start_y] = Cell_state::Empty;

  // For each neighboring cell...
  for (int i = 0; i < neighbour_offset_x.size(); ++i) {
    int new_x = start_x + neighbour_offset_x[i];
    int new_y = start_y + neighbour_offset_y[i];

    // If the neighboring cell is within the bounds of the board and has the
    // same symbol as the player_symbol...
    if (is_within_bounds(new_x, new_y) &&
        // Recursively perform a DFS from the neighboring cell.
        game_board_snapshot[new_x][new_y] == player_symbol &&
        depth_first_search(new_x, new_y, destination_x, destination_y,
                           player_symbol, game_board_snapshot)) {
      // If a path is found, restore the player's symbol in the current cell and
      // return True.
      game_board_snapshot[start_x][start_y] = player_symbol;
      return true;
    }
  }

  // If no path is found from the current cell, restore the player's symbol in
  // the current cell and return False.
  game_board_snapshot[start_x][start_y] = player_symbol;
  return false;
}

Cell_state Board::check_winner() const {
  // Check for player B (top to bottom)
  for (int top_row_index = 0; top_row_index < board_size; ++top_row_index) {
    if (board[0][top_row_index] == Cell_state::Blue) {
      for (int bottom_row_index = 0; bottom_row_index < board_size;
           ++bottom_row_index) {
        // Copy the current board to pass to the depth_first_search function
        std::vector<std::vector<Cell_state>> board_copy = board;
        // If a path is found between the top and bottom of the board for player
        // Blue, return Blue as the winner
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
        // Copy the current board to pass to the depth_first_search function
        std::vector<std::vector<Cell_state>> board_copy = board;
        // If a path is found between the left and right of the board for player
        // Red, return Red as the winner
        if (board[right_column_index][board_size - 1] == Cell_state::Red &&
            depth_first_search(left_column_index, 0, right_column_index,
                               board_size - 1, Cell_state::Red, board_copy)) {
          return Cell_state::Red;
        }
      }
    }
  }
  // If no paths are found for either player, return Empty to signify that there
  // is no winner yet
  return Cell_state::Empty;
}

void Board::display_board(std::ostream& os = std::cout) const {
  os << "\n";

  // Loop through each cell in the board.
  for (size_t row = 0; row < board_size; ++row) {
    // Indentation for the hexagonal pattern.
    os << std::string(2 * row, ' ');
    for (size_t col = 0; col < board_size; ++col) {
      // Print the state of the cell.
      os << board[row][col];
      // Print a line (-) between cells in the same row, except for the last
      // cell.
      if (col < board_size - 1) {
        os << " - ";
      }
    }
    // Print the row number at the end of each row.
    os << " " << row + 1;
    os << "\n";

    // Print lines (\ /) between cells in adjacent rows, except for the last
    // row.
    if (row < board_size - 1) {
      // Print the bottom coordinate labels (letters).
      os << std::string(2 * row + 1, ' ');
      for (size_t col = 0; col < board_size - 1; ++col) {
        os << "\\ / ";
      }
      os << "\\";
      os << "\n";
    }
  }
  // print the bottom coordinate labels
  os << std::string(2 * (board_size - 1) - 1, ' ');
  for (size_t col = 0; col < board_size; ++col) {
    os << "  " << static_cast<char>('a' + col) << " ";
  }
  os << "\n\n";
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
  // Call the display_board() function to print the board to the output stream.
  board.display_board(os);
  return os;
}
