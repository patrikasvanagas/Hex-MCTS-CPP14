#ifndef CONSOLE_INTERFACE_H
#define CONSOLE_INTERFACE_H

#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "game.h"

/**
 * @brief Function to get a yes or no response from the user.
 * @return Character 'y' or 'n' entered by the user.
 */
char get_yes_or_no_response();

/**
 * @brief Check if a string can be interpreted as an integer.
 * @param s String to check.
 * @return True if the string is an integer, false otherwise.
 */
bool is_integer(const std::string& s);

/**
 * @brief Create an MCTS agent for the game.
 * @param agent_prompt The prompt to display when creating the agent.
 * @return A unique_ptr to the created Mcts_player.
 */
std::unique_ptr<Mcts_player> create_mcts_agent(const std::string& agent_prompt);

/**
 * @brief Start a match against a robot player.
 */
void start_match_against_robot();

/**
 * @brief Start a robot vs robot match.
 */
void start_robot_arena();

/**
 * @brief Start a human vs human match.
 */
void start_human_arena();

/**
 * @brief Print the game documentation.
 */
void print_docs();

/**
 * @brief Print the exit message in ASCII art.
 */
void print_exit_ascii_art();

/**
 * @brief Run the console interface.
 */
void run_console_interface();

/**
 * @brief Check if a value is within bounds.
 * @param value The value to check.
 * @param lower_bound The lower bound of the range.
 * @param upper_bound The upper bound of the range.
 * @return True if the value is within bounds, false otherwise.
 */
template <typename T>
bool is_in_bounds(T value, T lower_bound, T upper_bound) 
{
  return value >= lower_bound && value <= upper_bound;
}

/**
 * @brief Get a parameter from the user that is within a specified range.
 * @param prompt The prompt to display to the user.
 * @param lower_bound The lower bound of the acceptable range.
 * @param upper_bound The upper bound of the acceptable range.
 * @return The user's input.
 */
template <typename T>
T get_parameter_within_bounds(const std::string& prompt, T lower_bound,
                              T upper_bound);

// Template specializations for get_parameter_within_bounds declarations
template <>
int get_parameter_within_bounds<int>(const std::string& prompt, int lower_bound,
                                     int upper_bound);

template <>
double get_parameter_within_bounds<double>(const std::string& prompt,
                                           double lower_bound,
                                           double upper_bound);

#endif  // CONSOLE_INTERFACE_H
