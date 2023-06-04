#include "player.h"

#include <chrono>
#include <iostream>

#include "mcts_agent.h"

/**
 * @brief Implementation of the choose_move function for the Human_player class.
 * This function prompts the user to input a row and column for their move.
 * It checks if the move is valid and, if not, prompts the user to try again.
 *
 * @param board The current state of the game board (Board).
 * @param player The current player (Cell_state).
 * @return The chosen move as a pair of integers, row first, column second.
 */
std::pair<int, int> Human_player::choose_move(const Board& board,
                                              Cell_state player) {
  int move_row;
  char move_col_symbol;  // column as a letter
  bool valid_move = false;

  while (!valid_move) {
    std::cout << "Enter the row as a number and the column as a letter "
                 "separated by space: ";
    if (!(std::cin >> move_row >> move_col_symbol)) {
      std::cout << "Invalid input! Try again." << std::endl;
      std::cin.clear();  // clear error flags
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                      '\n');  // ignore the rest of the line
      continue;
    }
    if (move_row < 1 || move_row > board.get_board_size()) {
      std::cout << "Invalid row! Try again." << std::endl;
      continue;
    }
    if (move_col_symbol >= 'a' &&
        move_col_symbol < 'a' + board.get_board_size()) {
      int move_col = move_col_symbol - 'a';
      if (board.is_valid_move(move_row - 1, move_col)) {
        return std::make_pair(move_row - 1, move_col);
      } else {
        std::cout << "Invalid move! Try again." << std::endl;
      }
    } else {
      std::cout << "Invalid column! Try again." << std::endl;
    }
  }
  return std::make_pair(-1, -1);  // should never reach this
}

/**
 * @brief Constructor for the Mcts_player class.
 * It initializes the exploration factor, maximum decision time, whether
 * computations are parallelized, and whether verbose logging is enabled.
 *
 * @param exploration_factor The exploration factor used in MCTS.
 * @param max_decision_time The maximum time allowed for decision making.
 * @param is_parallelized If true, MCTS computations are parallelized.
 * @param is_verbose If true, verbose logging is enabled.
 */
Mcts_player::Mcts_player(double exploration_factor,
                         std::chrono::milliseconds max_decision_time,
                         bool is_parallelized, bool is_verbose)
    : exploration_factor(exploration_factor),
      max_decision_time(max_decision_time),
      is_parallelized(is_parallelized),
      is_verbose(is_verbose) {}

/**
 * @brief Implementation of the choose_move function for the Mcts_player class.
 * This function uses the MCTS agent to choose a move. New instances of the
 * agent are created for each move, so the agent's tree is not preserved.
 *
 * @param board The current state of the game board.
 * @param player The current player.
 * @return The chosen move as a pair of integers.
 */
std::pair<int, int> Mcts_player::choose_move(const Board& board,
                                             Cell_state player) {
  Mcts_agent agent(exploration_factor, max_decision_time, is_parallelized,
                   is_verbose);
  return agent.choose_move(board, player);
}

/**
 * @brief Getter for the is_verbose private member of the Mcts_player class.
 *
 * @return The value of is_verbose.
 */
bool Mcts_player::get_is_verbose() const { return is_verbose; }