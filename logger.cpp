// logger.cpp

#include "logger.h"

// Initialize static member
std::shared_ptr<Logger> Logger::logger = nullptr;

std::shared_ptr<Logger> Logger::instance(bool is_verbose) {
  if (!logger) {
    logger = std::shared_ptr<Logger>(
        new Logger(is_verbose));  // initialize with the verbosity provided
  }
  return logger;
}

void Logger::log(const std::string& message) {
  std::lock_guard<std::mutex> lock(mutex);
  if (get_verbosity()) {
    std::cout << message << std::endl;
  }
}
