#ifndef CONSOLE_INTERFACE_H
#define CONSOLE_INTERFACE_H

#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "game.h"

char get_yes_or_no_response();
bool is_integer(const std::string& s);
void play_against_robot();
void print_welcome_ascii_art();
void start_robot_arena();
void start_human_arena();
void print_docs();
void print_exit_ascii_art();
void run_console_interface();

// Template function for checking if value is within bounds
template <typename T>
bool is_in_bounds(T value, T lower_bound, T upper_bound) {
  return value >= lower_bound && value <= upper_bound;
}

// Template function declaration
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
