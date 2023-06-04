#ifndef LOGGER_H
#define LOGGER_H

#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>

#include "board.h"
#include "cell_state.h"

/**
 * @class Logger
 *
 * @brief The Logger class provides a thread-safe, singleton logging utility
 *        for the Monte Carlo Tree Search (MCTS) algorithm used in the
 *        Mcts_agent class.
 *
 * The Logger class is designed as a singleton to ensure that only one logger
 * instance exists across the application, providing a unified source of
 * logging.
 *
 * This class is thread-safe, using a mutex to ensure that concurrent writes to
 * the console do not overlap.
 *
 * Logger provides various logging functions specific to different stages of the
 * MCTS algorithm, such as start and end of MCTS, expanding a child node,
 * selecting a child node, and so on. These functions are used to print the
 * status and progress of the MCTS algorithm and its internal state.
 *
 * The verbosity is set at the time of creating the logger instance.
 */
class Logger {
 public:
  /**
   * @brief Creates or retrieves the singleton Logger instance.
   *
   * If the Logger instance does not exist yet, it will be created with the
   * given verbosity. If it already exists, the verbosity will be ignored and
   * the existing instance will be returned.
   *
   * @param is_verbose Whether the logger should print verbose output.
   * @return A shared_ptr to the Logger instance.
   */
  static std::shared_ptr<Logger> instance(bool is_verbose);
  // Non-copyable and non-movable
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;
  Logger(bool verbose = false) : is_verbose(verbose) {}

  /**
   * @brief Returns the verbosity of the logger.
   *
   * @return The verbosity of the logger.
   */
  bool get_verbosity() const { return is_verbose; }

  /**
   * @brief Logs the start of an MCTS operation.
   *
   * @param player The player who is about to move.
   */
  void log_mcts_start(Cell_state player);

  /**
   * @brief Logs the start of a simulation iteration.
   *
   * @param iteration_number The number of the iteration.
   */
  void log_iteration_number(int iteration_number);

  /**
   * @brief Logs the expansion of a child node.
   *
   * @param move The move associated with the expanded child node.
   */
  void log_expanded_child(const std::pair<int, int>& move);

  /**
   * @brief Logs the selection of a child node and its UCT score.
   *
   * @param move The move associated with the selected child node.
   * @param uct_score The UCT score of the selected child node.
   */
  void log_selected_child(const std::pair<int, int>& move, double uct_score);

  /**
   * @brief Logs the start of a game simulation.
   *
   * @param move The move from which the simulation starts.
   * @param board The current state of the game board.
   */
  void log_simulation_start(const std::pair<int, int>& move,
                            const Board& board);
  /**
   * @brief Logs a step of a game simulation.
   *
   * @param current_player The current player.
   * @param board The current state of the game board.
   * @param move The move made by the current player.
   */
  void log_simulation_step(Cell_state current_player, const Board& board,
                           const std::pair<int, int>& move);
  /**
   * @brief Logs the end of a game simulation.
   *
   * @param winning_player The player who won the game.
   * @param board The final state of the game board.
   */
  void log_simulation_end(Cell_state winning_player, const Board& board);

  /**
   * @brief Logs the result of backpropagating a simulation result through the
   * tree.
   *
   * @param move The move associated with the node to which the result is
   * backpropagated.
   * @param win_count The current win count of the node.
   * @param visit_count The current visit count of the node.
   */
  void log_backpropagation_result(const std::pair<int, int>& move,
                                  int win_count, int visit_count);

  /**
   * @brief Logs the current stats of the root node.
   *
   * @param visit_count The current visit count of the root node.
   * @param win_count The current win count of the root node.
   * @param child_nodes The current number of child nodes of the root node.
   */
  void log_root_stats(int visit_count, int win_count, size_t child_nodes);

  /**
   * @brief Logs the current stats of a child node.
   *
   * @param move The move associated with the child node.
   * @param win_count The current win count of the child node.
   * @param visit_count The current visit count of the child node.
   */
  void log_child_node_stats(const std::pair<int, int>& move, int win_count,
                            int visit_count);

  /**
   * @brief Logs that the decision timer ran out.
   *
   * @param iteration_counter The number of iterations completed before the
   * timer ran out.
   */
  void log_timer_ran_out(int iteration_counter);

  /**
   * @brief Logs the current win ratio of a node.
   *
   * @param move The move associated with the node.
   * @param win_count The current win count of the node.
   * @param visit_count The current visit count of the node.
   */
  void log_node_win_ratio(const std::pair<int, int>& move, int win_count,
                          int visit_count);

  /**
   * @brief Logs the selection of the best child node at the end of the MCTS.
   *
   * @param iteration_counter The number of iterations completed.
   * @param move The move associated with the selected child node.
   * @param win_ratio The win ratio of the selected child node.
   */
  void log_best_child_chosen(int iteration_counter,
                             const std::pair<int, int>& move, double win_ratio);

  /**
   * @brief Logs the end of an MCTS operation.
   */
  void log_mcts_end();

 private:
  /**
   * @brief The singleton instance of the Logger class.
   */
  static std::shared_ptr<Logger> logger;

  /**
   * @brief A flag indicating whether the logger should print verbose messages.
   */
  bool is_verbose;

  /**
   * @brief Print a log message to the console.
   *
   * If `always_print` is false, the message is only printed if the logger is
   * in verbose mode.
   */
  void log(const std::string& message, bool always_print);

  /**
   * @brief Mutex for ensuring thread safety when logging.
   */
  std::mutex mutex;
};

#endif  // LOGGER_H
