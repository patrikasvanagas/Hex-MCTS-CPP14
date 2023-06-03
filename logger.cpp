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

void Logger::log_mcts_start(Cell_state player) {
  std::stringstream message;
  message << "\n-------------MCTS VERBOSE START - " << player
          << " to move-------------\n";
  log(message.str());
}

void Logger::log_iteration_number(int iteration_number) {
  std::stringstream message;
  message << "\n------------------STARTING SIMULATION " << iteration_number
          << "------------------\n";
  log(message.str());
}

void Logger::log_selected_child(const std::pair<int, int>& move,
                                double uct_score) {
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


