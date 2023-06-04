#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "cell_state.h"

/**
 * @brief The Board class represents the game board for a game of Hex.
 *
 * Hex is a strategy board game for two players. The players alternate placing
 * pieces on the board, with the goal of creating a connected path of their
 * pieces between two opposite sides of the board.
 *
 * The Board class provides functionality for managing and interacting with the
 * game board, including:
 * - Initializing the board with a given size.
 * - Displaying the board in the console.
 * - Checking if a cell is within the bounds of the board.
 * - Checking if a move is valid (i.e., within the bounds of the board and to an
 * empty cell).
 * - Making a move on the board.
 * - Checking if there is a winner (i.e., if a player has a connected path of
 * their pieces across the board).
 * - Getting a list of all valid moves on the board.
 * - Checking if two cells on the board are connected.
 *
 * The Board class also overloads the << operator to enable printing the board
 * directly to an output stream.
 *
 * The board is represented internally as a 2D vector of Cell_state enums. The
 * Cell_state enum represents the state of a cell on the board (empty, occupied
 * by player 1, or occupied by player 2).
 *
 * Note: This class does not handle player turns or game logic beyond the
 * mechanics of the game board itself.
 */
class Board {
 public:
  /**
   * @brief Constructor for Board class.
   *
   * @param size: Integer to set the size of the board.
   */
  Board(int size);

  /**
   * @brief Getter for the size of the board.
   *
   * @return The size of the board.
   */
  int get_board_size() const;

  /**
   * @brief Checks if a given cell, identified by its x and y coordinates, is
   * within the bounds of the board.
   * @param move_x: The x-coordinate of the cell.
   * @param move_y: The y-coordinate of the cell.
   * @return True if the cell is within the bounds of the board, else False.
   */
  bool is_within_bounds(int move_x, int move_y) const;

  /**
   * @brief Checks if a move is valid. A move is valid if it is within the
   * bounds of the board and the target cell is empty.
   * @param move_x: The x-coordinate of the cell.
   * @param move_y: The y-coordinate of the cell.
   * @return True if the move is valid, else False.
   */
  bool is_valid_move(int move_x, int move_y) const;

  /**
   * @brief Get all valid moves on the board.
   *
   * This function iterates over each cell on the board and checks if a move at
   * that position is valid. If the move is valid, it adds the position of the
   * cell to a vector.
   *
   * @return A vector of pairs, where each pair represents the row and column
   * indices of a valid move on the board.
   */
  std::vector<std::pair<int, int>> get_valid_moves() const;

  /**
   * @brief Makes a move on the board on behalf of a player. The move is made at
   * the specified x and y coordinates. If the move is invalid, an exception is
   * thrown.
   *
   * @param move_x: The x-coordinate (row) of the move.
   * @param move_y: The y-coordinate (column) of the move.
   * @param player: The player making the move (Cell_state).
   *
   * @exception std::invalid_argument If the move is invalid, i.e., it's outside
   * the board boundaries or the target cell is not empty.
   */
  void make_move(int move_x, int move_y, Cell_state player);

  /**
   * @brief Checks if two cells on the board are connected.
   *
   * This function checks if two given cells on the board are connected by
   * comparing the coordinates of the second cell to the coordinates of all the
   * neighboring cells of the first cell. In the game of Hex, two cells are
   * considered connected if they are adjacent to each other in any of the six
   * directions.
   *
   * @param first_cell_x: The x-coordinate (row) of the first cell.
   * @param first_cell_y: The y-coordinate (column) of the first cell.
   * @param second_cell_x: The x-coordinate (row) of the second cell.
   * @param second_cell_y: The y-coordinate (column) of the second cell.
   *
   * @return True if the two cells are connected, else False.
   *
   * @notes Kept only for debugging and not used in the game.
   */
  bool are_cells_connected(int first_cell_x, int first_cell_y,
                           int second_cell_x, int second_cell_y) const;

  /**
   * @brief A Depth-First Search (DFS) algorithm to check if there is a path
   * from the start cell to the destination cell.
   *
   * The DFS is performed recursively, starting from the start cell and
   * exploring the neighboring cells. A cell is considered reachable if it is
   * within the bounds of the board and has the same symbol as the
   * player_symbol. During the search, the explored cells are temporarily marked
   * as empty in the game_board_snapshot to prevent loops.
   *
   * @param start_x: The x-coordinate (row) of the starting cell for the DFS.
   * @param start_y: The y-coordinate (column) of the starting cell for the DFS.
   * @param destination_x: The x-coordinate (row) of the destination cell.
   * @param destination_y: The y-coordinate (column) of the destination cell.
   * @param player_symbol: The symbol (Cell_state) of the player for whom the
   * path is being checked.
   * @param game_board_snapshot: A snapshot of the game board state, represented
   * as a 2D vector. It is used to keep track of the cells that have been
   * explored during the DFS. It is passed by reference to avoid copying the
   * entire game board at each recursive call.
   *
   * @return True if there is a path from the start cell to the destination
   * cell, else False.
   */
  bool depth_first_search(
      int start_x, int start_y, int destination_x, int destination_y,
      Cell_state player_symbol,
      std::vector<std::vector<Cell_state>>& game_board_snapshot) const;

  /**
   * @brief Checks if there is a winner in the game.
   *
   * This function checks for a winning path for both players (Blue and Red).
   * For Blue, it checks for a path from any cell in the top row to any cell in
   * the bottom row. For Red, it checks for a path from any cell in the leftmost
   * column to any cell in the rightmost column. The function utilizes the
   * depth_first_search method to find a path.
   *
   * @return The Cell_state of the winning player. If there is no winner, it
   * returns Cell_state::Empty.
   */
  Cell_state check_winner() const;

  /**
   * @brief Outputs the current state of the board to an output stream.
   * The board is displayed in a hexagonal pattern, with each cell represented
   * by its current Cell_state (Empty, Player1, Player2). Each cell is connected
   * to its neighbors with lines (- and \ /). The board is also labeled with
   * numbers for rows and letters for columns for easier identification of
   * cells.
   * @param os: The output stream to which the board state is to be printed.
   * Defaults to std::cout if not provided.
   */
  void display_board(std::ostream& os) const;

  /**
   * @brief Overloads the << operator for the Board class. This function allows
   * the board to be directly printed to an output stream (such as std::cout) by
   * calling the display_board() method of the Board class.
   * @param os: The output stream to which the board state is to be printed.
   * @param board: The board to be printed.
   * @return The output stream with the board state appended.
   */
  friend std::ostream& operator<<(std::ostream& os, const Board& board);

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
};

#endif
