#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "cell_state.h"

class Board {
 public:
  Board(int size);
  void display_board() const;
  bool is_valid_move(int move_x, int move_y) const;
  void make_move(int move_x, int move_y, Cell_state player);
  Cell_state check_winner() const;
  int get_board_size() const;
  friend std::ostream& operator<<(std::ostream& os, const Board& board);
  std::vector<std::pair<int, int>> get_valid_moves() const;

 private:
  int board_size;
  std::vector<std::vector<Cell_state>> board;
  std::array<int, 6> neighbour_offset_x = {-1, -1, 0, 1, 1, 0};
  std::array<int, 6> neighbour_offset_y = {0, 1, 1, 0, -1, -1};
  bool is_within_bounds(int move_x, int move_y) const;
  bool depth_first_search(
      int start_x, int start_y, int destination_x, int destination_y,
      Cell_state player_symbol,
      std::vector<std::vector<Cell_state>>& game_board_snapshot) const;
  // Kept for debugging
  bool are_cells_connected(int first_cell_x, int first_cell_y,
                           int second_cell_x, int second_cell_y) const;
};

#endif