#ifndef CONSOLE_INTERFACE_H
#define CONSOLE_INTERFACE_H

#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "game.h"

/**
 * @brief Checks if the input string is an integer.
 *
 * This function checks if all characters in a string are digits,
 * thus determining if the string represents a whole number.
 *
 * @param s The string to check.
 * @return true if the string is an integer, false otherwise.
 */
bool is_integer(const std::string& s);

/**
 * @brief Prompt the user for a 'yes' or 'no' response.
 *
 * This function takes a prompt string, displays it, and reads the user's
 * response. It ensures the user's input is valid ('y', 'n', 'Y', or 'N') before
 * returning.
 *
 * @param prompt The string to display to the user.
 * @return Lowercase 'y' or 'n' based on user's response.
 */
char get_yes_or_no_response();

/**
 * @brief Check if a value is within bounds.
 * @param value The value to check.
 * @param lower_bound The lower bound of the range.
 * @param upper_bound The upper bound of the range.
 * @return True if the value is within bounds, false otherwise.
 */
template <typename T>
bool is_in_bounds(T value, T lower_bound, T upper_bound) {
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

/**
 * @brief Template specializations of get_parameter_within_bounds for int and
 * double types.
 *
 * These functions prompt the user for an input within specified bounds.
 * The user is repeatedly asked until a valid response is given.
 *
 * @param prompt The string to display to the user.
 * @param lower_bound The inclusive lower bound.
 * @param upper_bound The inclusive upper bound.
 * @return The user's input as an int or a double.
 */
template <>
int get_parameter_within_bounds<int>(const std::string& prompt, int lower_bound,
                                     int upper_bound);

template <>
double get_parameter_within_bounds<double>(const std::string& prompt,
                                           double lower_bound,
                                           double upper_bound);

/**
 * @brief Creates a Monte Carlo Tree Search (MCTS) player with custom
 * parameters.
 *
 * This function prompts the user for various parameters to initialize the MCTS
 * agent, such as maximum decision time, exploration constant, parallelization,
 * and verbosity.
 *
 * @param agent_prompt The string used to indicate the agent being initialized.
 * @return A unique pointer to the MCTS agent.
 */
std::unique_ptr<Mcts_player> create_mcts_agent(const std::string& agent_prompt);

/**
 * @brief A simple countdown function.
 *
 * This function performs a countdown from the specified number of seconds to 0.
 * It is used before the start of Robot vs Human game when verbose mode is
 * enabled.
 *
 * @param seconds The number of seconds to count down from.
 */
void countdown(int seconds);

/**
 * @brief Start a game against an MCTS agent.
 *
 * This function prompts the user for the desired player number and board size.
 * It then creates an MCTS agent and a human player, and starts a game.
 */
void start_match_against_robot();

/**
 * @brief Start a game between two MCTS agents.
 *
 * This function prompts the user for the board size, creates two MCTS agents,
 * and starts a game between them.
 */
void start_robot_arena();

/**
 * @brief Start a game between two human players.
 *
 * This function prompts the user for the board size, creates two human players,
 * and starts a game between them.
 */
void start_human_arena();

/**
 * @brief Run the console interface of the game.
 *
 * This function drives the main loop of the console interface.
 * It displays a menu and handles user's selection,
 * allowing the user to start different types of games or read the game docs.
 * It also handles exceptions and displays appropriate error messages.
 */
void run_console_interface();

/**
 * @brief Prints welcome message in ASCII art.
 */
void print_welcome_ascii_art();

/**
 * @brief Displays the Board object and the winner
 * from the pieces on the board.
 *
 * This function is used for demonstration purposes to show a winning condition.
 *
 * @param board The game board to display.
 */
void print_board_and_winner(Board& board);

/**
 * @brief Allows to build Hex board configurations
 * to show the winning condition and calls
 * print_board_and_winner to display them.
 *
 * This function is used for demonstration purposes to show a winning condition.
 *
 */
void display_winning_condition();

/**
 * @brief Displays some information about the
 * programme.
 *
 * Prints out a brief explanation of the Hex game,
 * how it is displayed in the console, and MCTS
 * agents.
 */
void print_docs();

/**
 * @brief Prints exit message in ASCII art.
 */
void print_exit_ascii_art();

#endif  // CONSOLE_INTERFACE_H