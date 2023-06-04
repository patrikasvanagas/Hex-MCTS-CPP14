#ifndef LOGGER_H
#define LOGGER_H

#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>

#include "board.h"
#include "cell_state.h"

class Logger {
 public:
  // Return the instance of the Logger (creates it if it doesn't exist)
  static std::shared_ptr<Logger> instance(bool is_verbose);
  // Non-copyable and non-movable
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;
  Logger(bool verbose = false) : is_verbose(verbose) {}
  // void set_verbosity(bool verbose) { is_verbose = verbose; }
  bool get_verbosity() const { return is_verbose; }

  void log_mcts_start(Cell_state player);
  void log_iteration_number(int iteration_number);

  void log_expanded_child(const std::pair<int, int>& move);

  void log_selected_child(const std::pair<int, int>& move, double uct_score);

  void log_simulation_start(const std::pair<int, int>& move,
                            const Board& board);
  void log_simulation_step(Cell_state current_player, const Board& board,
                           const std::pair<int, int>& move);
  void log_simulation_end(Cell_state winning_player, const Board& board);

  void log_backpropagation_result(const std::pair<int, int>& move,
                                  int win_count, int visit_count);

  void log_root_stats(int visit_count, int win_count, size_t child_nodes);
  void log_child_node_stats(const std::pair<int, int>& move, int win_count,
                            int visit_count);

  void log_timer_ran_out(int iteration_counter);

  void log_node_win_ratio(const std::pair<int, int>& move, int win_count,
                          int visit_count);

  void log_best_child_chosen(int iteration_counter,
                             const std::pair<int, int>& move, double win_ratio);

  void log_mcts_end();

 private:
  // Singleton instance
  static std::shared_ptr<Logger> logger;
  bool is_verbose;
  void log(const std::string& message, bool always_print);
  // Mutex for thread safety
  std::mutex mutex;
};

#endif  // LOGGER_H
