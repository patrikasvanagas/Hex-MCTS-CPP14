// logger.h

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <memory>
#include <mutex>

class Logger 
{
 public:
  // Return the instance of the Logger (creates it if it doesn't exist)
  static std::shared_ptr<Logger> instance();

  // Non-copyable and non-movable
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;

  Logger(bool verbose = false) : is_verbose(verbose) {}

  void set_verbosity(bool verbose) { is_verbose = verbose; }

  // Log a message to the console
  void log(const std::string& message);

 private:

  // Singleton instance
  static std::shared_ptr<Logger> logger;

  bool is_verbose;
  bool get_verbosity() const { return is_verbose; }
  // Mutex for thread safety
  std::mutex mutex;
};

#endif  // LOGGER_H
