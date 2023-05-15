#include "board.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "iterator"

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
Board::Board(int size)
    : board_size(size),
      board(size, std::vector<Cell_state>(size, Cell_state::Empty)) 
{
  if (size < 2) {
    throw std::invalid_argument("Board size cannot be less than 2.");
  }
}

/**
 * @brief Displays the current state of the board on the console.
 * The board is printed in a hexagonal pattern, with each cell represented by
 * its current Cell_state (Empty, Player1, Player2). Each cell is connected to its
 * neighbors with lines (- and \ /). The board is also labeled with numbers for
 * rows and letters for columns for easier identification of cells.
 */
void Board::display_board() const 
{
  std::cout << "\n";

  // Loop through each cell in the board.
  for (size_t row = 0; row < board_size; ++row) {
    // Indentation for the hexagonal pattern.
    std::cout << std::string(2 * row, ' ');
    for (size_t col = 0; col < board_size; ++col) {
      // Print the state of the cell.
      std::cout << board[row][col];
      // Print a line (-) between cells in the same row, except for the last
      // cell.
      if (col < board_size - 1) {
        std::cout << " - ";
      }
    }
    // Print the row number at the end of each row.
    std::cout << " " << row + 1;
    std::cout << "\n";

    // Print lines (\ /) between cells in adjacent rows, except for the last
    // row.
    if (row < board_size - 1) {
      // Print the bottom coordinate labels (letters).
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

/**
 * @brief Overloads the << operator for the Board class. This function allows
 * the board to be directly printed to an output stream (such as std::cout) by
 * calling the display_board() method of the Board class.
 * @param os: The output stream.
 * @param board: The board to be printed.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os, const Board& board) 
{
  // Call the display_board() function to print the board to the output stream.
  board.display_board();
  return os;
}

/**
 * @brief Checks if a given cell, identified by its x and y coordinates, is
 * within the bounds of the board.
 * @param move_x: The x-coordinate of the cell.
 * @param move_y: The y-coordinate of the cell.
 * @return True if the cell is within the bounds of the board, else False.
 */
bool Board::is_within_bounds(int move_x, int move_y) const 
{
  return move_x >= 0 && move_x < board_size && move_y >= 0 &&
         move_y < board_size;
}

/**
 * @brief Checks if a move is valid. A move is valid if it is within the bounds
 * of the board and the target cell is empty.
 * @param move_x: The x-coordinate of the cell.
 * @param move_y: The y-coordinate of the cell.
 * @return True if the move is valid, else False.
 */
bool Board::is_valid_move(int move_x, int move_y) const 
{
  return is_within_bounds(move_x, move_y) &&
         (board[move_x][move_y] == Cell_state::Empty);
}

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
void Board::make_move(int move_x, int move_y, Cell_state player) 
{
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

/**
 * @brief Getter for the size of the board.
 *
 * @return The size of the board.
 */
int Board::get_board_size() const { return board_size; }
int Board::get_board_size() const 
{ 
    return board_size; 
}

/**
 * @brief A Depth-First Search (DFS) algorithm to check if there is a path from
 * the start cell to the destination cell.
 *
 * The DFS is performed recursively, starting from the start cell and exploring
 * the neighboring cells. A cell is considered reachable if it is within the
 * bounds of the board and has the same symbol as the player_symbol. During the
 * search, the explored cells are temporarily marked as empty in the
 * game_board_snapshot to prevent loops.
 *
 * @param start_x: The x-coordinate (row) of the starting cell for the DFS.
 * @param start_y: The y-coordinate (column) of the starting cell for the DFS.
 * @param destination_x: The x-coordinate (row) of the destination cell.
 * @param destination_y: The y-coordinate (column) of the destination cell.
 * @param player_symbol: The symbol (Cell_state) of the player for whom the path is
 * being checked.
 * @param game_board_snapshot: A snapshot of the game board state, represented
 * as a 2D vector. It is used to keep track of the cells that have been explored
 * during the DFS. It is passed by reference to avoid copying the entire game
 * board at each recursive call.
 *
 * @return True if there is a path from the start cell to the destination cell,
 * else False.
 */
bool Board::depth_first_search(
    int start_x, int start_y, int destination_x, int destination_y,
    Cell_state player_symbol,
    std::vector<std::vector<Cell_state>>& game_board_snapshot) const 
{
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
Cell_state Board::check_winner() const 
{
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
std::vector<std::pair<int, int>> Board::get_valid_moves() const 
{
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
bool Board::are_cells_connected(int first_cell_x, int first_cell_y,
                                int second_cell_x, int second_cell_y) const 
{
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
