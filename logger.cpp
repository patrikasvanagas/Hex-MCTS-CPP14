// logger.cpp

#include "logger.h"

// Initialize static member
std::shared_ptr<Logger> Logger::logger = nullptr;

std::shared_ptr<Logger> Logger::instance(bool is_verbose) 
{
  if (!logger) {
    logger = std::make_shared<Logger>(
        is_verbose);  // initialize with the verbosity provided
  }
  return logger;
}

void Logger::log(const std::string& message) 
{
  std::lock_guard<std::mutex> lock(mutex);
  if (get_verbosity()) {
    std::cout << message << std::endl;
  }
}

void Logger::log_mcts_start(Cell_state player) 
{
  std::stringstream message;
  message << "\n-------------MCTS VERBOSE START - " << player
          << " to move-------------\n";
  log(message.str());
}

void Logger::log_iteration_number(int iteration_number) 
{
  std::stringstream message;
  message << "\n------------------STARTING SIMULATION " << iteration_number
          << "------------------\n";
  log(message.str());
}

void Logger::log_selected_child(const std::pair<int, int>& move,
                                double uct_score) 
{
  std::stringstream message;
  message << "\nSELECTED CHILD " << move.first << ", " << move.second
          << " with UCT of ";
  if (uct_score == std::numeric_limits<double>::max()) {
    message << "infinity";
  } else {
    message << std::setprecision(4) << uct_score;
  }
  log(message.str());
}

void Logger::log_simulation_start(const std::pair<int, int>& move,
                                  const Board& board) 
{
  std::stringstream message;
  message << "\nSIMULATING A RANDOM PLAYOUT from node " << move.first << ", "
          << move.second << ". Simulation board is in state:\n"
          << board;
  log(message.str());
}

void Logger::log_simulation_step(Cell_state current_player, const Board& board,
                                 const std::pair<int, int>& move) 
{
  std::stringstream message;
  message << "Current player in simulation is " << current_player
          << " in Board state:\n"
          << board;
  message << current_player << " makes random move " << move.first << ","
          << move.second << ". ";
  log(message.str());
}

void Logger::log_simulation_end(Cell_state winning_player, const Board& board) 
{
  std::stringstream message;
  message << "DETECTED WIN for player " << winning_player
          << " in Board state:\n"
          << board << "\n";
  log(message.str());
}

void Logger::log_backpropagation_result(const std::pair<int, int>& move,
                                        int win_count, int visit_count) 
{
  std::stringstream message;
  message << "BACKPROPAGATED result to node " << move.first << ", "
          << move.second << ". It currently has " << win_count << " wins and "
          << visit_count << " visits.";
  log(message.str());
}

void Logger::log_root_stats(int visit_count, int win_count,
                            size_t child_nodes) 
{
  std::stringstream message;
  message << "\nAFTER BACKPROPAGATION, root node has " << visit_count
          << " visits, " << win_count << " wins, and " << child_nodes
          << " child nodes. Their details are:\n";
  log(message.str());
}

void Logger::log_child_node_stats(const std::pair<int, int>& move,
                                  int win_count, int visit_count) 
{
  std::stringstream message;
  message << "Child node " << move.first << "," << move.second
          << ": Wins: " << win_count << ", Visits: " << visit_count
          << ". Win ratio: ";

  if (visit_count) {
    message << std::fixed << std::setprecision(2)
            << static_cast<double>(win_count) / visit_count;
  } else {
    message << "N/A (no visits yet)";
  }

  log(message.str());
}

void Logger::log_timer_ran_out(int iteration_counter) 
{
  std::stringstream message;
  message << "\nTIMER RAN OUT. " << iteration_counter
          << " iterations completed. CHOOSING A MOVE FROM ROOT'S CHILDREN:";
  log(message.str());
}

void Logger::log_node_win_ratio(const std::pair<int, int>& move, int win_count,
                           int visit_count) 
{
  std::ostringstream win_ratio_stream;
  if (visit_count > 0) {
    win_ratio_stream << std::fixed << std::setprecision(2)
                     << static_cast<double>(win_count) / visit_count;
  } else {
    win_ratio_stream << "N/A (no visits yet)";
  }

  std::stringstream message;
  message << "Child " << move.first << "," << move.second
          << " has a win ratio of " << win_ratio_stream.str();
  log(message.str());
}

void Logger::log_best_child_chosen(int iteration_counter,
                                   const std::pair<int, int>& move,
                                   double win_ratio) 
{
  std::stringstream message;
  message << "\nAfter " << iteration_counter << " iterations, choose child "
          << move.first << ", " << move.second << " with win ratio "
          << std::setprecision(4) << win_ratio;
  log(message.str());
}

void Logger::log_mcts_end() 
{
  log("\n--------------------MCTS VERBOSE END--------------------\n");
}

