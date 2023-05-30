#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "cell_state.h"

/**
 * @brief The Board class is responsible for maintaining and manipulating the
 * Hex board state. It provides functions to make a move, check for a winner,
 * validate moves, and display the current state of the board. The board is
 * represented as a 2D vector of Cell_state enums.
 */
class Board 
{
 public:
  /**
   * @brief Constructor for Board class.
   * @param size: Integer to set the size of the board.
   */
  Board(int size);

  /**
   * @brief Displays the current state of the game board.
   */
  void display_board() const;

  /**
   * @brief Checks if a move is valid, i.e., within the board boundaries and not
   * already occupied.
   * @param move_x: The x-coordinate of the move.
   * @param move_y: The y-coordinate of the move.
   * @return True if the move is valid, else False.
   */
  bool is_valid_move(int move_x, int move_y) const;

  /**
   * @brief Makes a move on the board.
   * @param move_x: The x-coordinate (row) of the move.
   * @param move_y: The y-coordinate (column) of the move.
   * @param player: The player making the move (Cell_state).
   */
  void make_move(int move_x, int move_y, Cell_state player);

  /**
   * @brief Checks if there is a winner.
   * @return The player who has won the game. If there is no winner, returns
   * Cell_state::Empty.
   */
  Cell_state check_winner() const;

    /**
   * @brief Getter for the size of the board.
   * @return The size of the board.
   */
  int get_board_size() const;

   /**
   * @brief Operator overload for ostream. Used to print the board.
   * @param os: The output stream.
   * @param board: The board to be printed.
   * @return The output stream.
   */
  friend std::ostream& operator<<(std::ostream& os, const Board& board);

   /**
   * @brief Gets the valid moves that can be made on the board.
   * @return A vector of valid moves.
   */
  std::vector<std::pair<int, int>> get_valid_moves() const;

 private:
  /**
   * @brief The size of the board.
   */
  int board_size;

   /**
   * @brief A 2D vector representing the game board. Each Cell_state
   * signifies the state of a cell in the board - it can be either empty,
   * or occupied by one of the two players.
   */
  std::vector<std::vector<Cell_state>> board;

   /**
   * @brief An array storing the x offsets for the six possible directions
   * in the Hex game. It is used to find neighbouring cells on the board.
   */
  std::array<int, 6> neighbour_offset_x = {-1, -1, 0, 1, 1, 0};

   /**
   * @brief An array storing the y offsets for the six possible directions
   * in the Hex game. It is used to find neighbouring cells on the board.
   */
  std::array<int, 6> neighbour_offset_y = {0, 1, 1, 0, -1, -1};
  // Private helper functions for Board. More extensive docs in board.cpp.

   /**
   * @brief Checks if a given move is within the boundaries of the board.
   * @param move_x: The x-coordinate (row) of the move.
   * @param move_y: The y-coordinate (row) of the move.
   * @return True if the move is within the board, else False.
   */
  bool is_within_bounds(int move_x, int move_y) const;
  

  /**
   * @brief Performs a depth-first search from a start cell to a destination
   * cell.
   * @param start_x: The x-coordinate (row) of the start cell.
   * @param start_y: The y-coordinate (column) of the start cell.
   * @param destination_x: The x-coordinate (column) of the destination cell.
   * @param destination_y: The y-coordinate (row)  of the destination cell.
   * @param player_symbol: The player's symbol (Cell_state).
   * @param game_board_snapshot: The current state of the game board.
   * @return True if there is a path from the start cell to the destination
   * cell, else False.
   */
  bool depth_first_search(
      int start_x, int start_y, int destination_x, int destination_y,
      Cell_state player_symbol,
      std::vector<std::vector<Cell_state>>& game_board_snapshot) const;

  /**
   * @brief Checks if two cells are connected (for debugging).
   * @param first_cell_x: The x-coordinate of the first cell.
   * @param first_cell_y: The y-coordinate of the first cell.
   * @param second_cell_x: The x-coordinate of the second cell.
   * @param second_cell_y: The y-coordinate of the second cell.
   * @return True if the cells are connected, else False.
   */
  bool are_cells_connected(int first_cell_x, int first_cell_y,
                           int second_cell_x, int second_cell_y) const;
};

#endif
