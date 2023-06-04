#ifndef PLAYER_H
#define PLAYER_H

#include <chrono>
#include <utility>

#include "board.h"

/**
 * @brief Player serves as an abstract base class providing a contract for all
 * derived player types in a game. It encapsulates the generic behaviors and
 * attributes of a player. All specific types of players (like Human_player,
 * Mcts_player etc.) must inherit from this class and define their unique
 * behaviors by implementing the abstract methods.
 *
 * A Player's primary responsibility is to choose a move based on the current
 * state of the game board. This interaction is modelled via the pure virtual
 * function choose_move().
 */
class Player {
 public:
  /**
   * @brief Abstract function for choosing a move on the game board.
   * @param board Current state of the game board (Board).
   * @param player The player who is making the move (Cell_state).
   * @return The chosen move as a pair of integers, row first, column second.
   */
  virtual std::pair<int, int> choose_move(const Board& board,
                                          Cell_state player) = 0;
};

/**
 * @brief Human_player is a concrete class derived from the Player base class.
 * It represents a human player in a game, implementing the necessary behaviors
 * for move selection and interaction with the game.
 *
 * The choose_move() function is implemented to accept input directly from the
 * user. The input is then checked for validity before being accepted as a
 * legitimate move.
 */
class Human_player : public Player {
 public:
  /**
   * @brief Implementation of the choose_move function for the Human_player
   * class. This function prompts the user to input a row and column for their
   * move. It checks if the move is valid and, if not, prompts the user to try
   * again.
   *
   * @param board The current state of the game board (Board).
   * @param player The current player (Cell_state).
   * @return The chosen move as a pair of integers, row first, column second.
   */
  std::pair<int, int> choose_move(const Board& board,
                                  Cell_state player) override;
};

/**
 * @brief Mcts_player is a concrete class derived from the Player base class,
 * embodying a player that utilizes the Monte Carlo Tree Search (MCTS) algorithm
 * for decision making during a game.
 *
 * The choose_move() function is implemented to utilize an MCTS agent for
 * determining the best move. This is based on the exploration factor, maximum
 * decision time, and whether computations are parallelized and verbose logging
 * is enabled. All these parameters are customizable during the instantiation
 * of a Mcts_player.
 */
class Mcts_player : public Player {
 public:
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
  Mcts_player(double exploration_factor,
              std::chrono::milliseconds max_decision_time,
              bool is_parallelized = false, bool is_verbose = false);

  /**
   * @brief Implementation of the choose_move function for the Mcts_player
   * class. This function uses the MCTS agent to choose a move. New instances of
   * the agent are created for each move, so the agent's tree is not preserved.
   *
   * @param board The current state of the game board.
   * @param player The current player.
   * @return The chosen move as a pair of integers.
   */
  std::pair<int, int> choose_move(const Board& board,
                                  Cell_state player) override;

  /**
   * @brief Getter for the is_verbose private member of the Mcts_player class.
   *
   * @return The value of is_verbose.
   */
  bool get_is_verbose() const;

 private:
  double exploration_factor;  // The exploration factor used in MCTS.
  std::chrono::milliseconds max_decision_time;  // Maximum decision-making time.
  bool is_parallelized;  // If true, MCTS computations are parallelized.
  bool is_verbose;       // If true, enables verbose logging to console.
};

#endif