// logger.cpp

#include "logger.h"

// Initialize static member
std::shared_ptr<Logger> Logger::logger = nullptr;

std::shared_ptr<Logger> Logger::instance() {
  if (!logger) {
    logger = std::shared_ptr<Logger>(
        new Logger(false));  // initialize with verbosity = false
  }
  return logger;
}

void Logger::log(const std::string& message) {
  std::lock_guard<std::mutex> lock(mutex);
  if (get_verbosity()) {
    std::cout << message << std::endl;
  }
}
