#ifndef PLAYER_H
#define PLAYER_H

#include <chrono>
#include <utility>

#include "board.h"

/**
 * @brief Player is an abstract base class that represents a generic player.
 * A player can choose a move given the state of a game board.
 */
class Player 
{
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
 * @brief Human_player is a concrete class derived from Player that represents a
 * human player.
 */
class Human_player : public Player 
{
 public:
  /**
   * @brief Override function for a human player to choose a move on the game
   * board.
   * @param board Current state of the game board (Board).
   * @param player The player who is making the move (Cell_state).
   * @return The chosen move as a pair of integers, row first, column second.
   */
  std::pair<int, int> choose_move(const Board& board,
                                  Cell_state player) override;
};

/**
 * @brief Mcts_player is a concrete class derived from Player that represents a
 * player using Monte Carlo Tree Search for decision making.
 */
class Mcts_player : public Player 
{
 public:
  /**
   * @brief Mcts_player constructor.
   * @param exploration_factor The exploration factor used in MCTS.
   * @param max_decision_time The maximum time allowed for decision making.
   * @param is_parallelized Indicates if the MCTS computations should be
   * parallelized.
   * @param is_verbose Indicates if verbose logging should be used.
   */
  Mcts_player(double exploration_factor,
              std::chrono::milliseconds max_decision_time,
              bool is_parallelized = false, bool is_verbose = false);
  std::pair<int, int> choose_move(const Board& board,
                                  Cell_state player) override;

  /**
   * @brief Getter for the is_verbose private member.
   * @return The value of is_verbose.
   */
  bool get_is_verbose() const;

 private:
  double exploration_factor; // The exploration factor used in MCTS.
  std::chrono::milliseconds max_decision_time; // Maximum decision-making time.
  bool is_parallelized; // If true, MCTS computations are parallelized.
  bool is_verbose; // If true, enables verbose logging to console.
};

#endif