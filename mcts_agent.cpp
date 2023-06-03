#include "mcts_agent.h"

#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>

Mcts_agent::Mcts_agent(double exploration_factor,
                       std::chrono::milliseconds max_decision_time,
                       bool is_parallelized, bool is_verbose)
    : exploration_factor(exploration_factor),
      max_decision_time(max_decision_time),
      logger(Logger::instance(is_verbose)),
      random_generator(random_device()) {
  if (is_parallelized && is_verbose) {
    throw std::logic_error(
        "Concurrent playouts and verbose mode do not make sense together.");
  }
}

Mcts_agent::Node::Node(Cell_state player, std::pair<int, int> move,
                       std::shared_ptr<Node> parent_node)
    : win_count(0),
      visit_count(0),
      move(move),
      player(player),
      child_nodes(),
      parent_node(parent_node) {}

std::pair<int, int> Mcts_agent::choose_move(const Board& board,
                                            Cell_state player) {
  logger->log_mcts_start(player);
  // Create a new root node for MCTS
  root = std::make_shared<Node>(player, std::make_pair(-1, -1), nullptr);
  // Prepare for potential parallelism
  std::vector<std::thread> threads;
  unsigned int number_of_threads;
  if (is_parallelized) {
    // Determine the maximum number of threads available on the hardware.
    number_of_threads = std::thread::hardware_concurrency();
  }
  if (!is_verbose) {
    std::cout << "Thinking silently..." << std::endl;
  }
  // Expand root based on the current game state
  expand_node(root, board);
  int mcts_iteration_counter = 0;
  auto start_time = std::chrono::high_resolution_clock::now();
  auto end_time = start_time + max_decision_time;
  // The main loop of the MCTS. It runs until the decision-making time limit is
  // reached.
  while (std::chrono::high_resolution_clock::now() < end_time) {
    logger->log_iteration_number(mcts_iteration_counter + 1);
    std::shared_ptr<Node> chosen_child = select_child(root);
    if (is_parallelized) {
      // Start the threads and put their separate results into a vector:
      std::vector<Cell_state> results(number_of_threads);
      for (unsigned int thread_index = 0; thread_index < number_of_threads;
           thread_index++) {
        threads.push_back(std::thread([&, thread_index]() {
          results[thread_index] = simulate_random_playout(chosen_child, board);
        }));
      }
      // Join the threads:
      for (auto& thread : threads) {
        thread.join();
      }
      threads.clear();
      // Backpropagate each of the results:
      for (Cell_state playout_winner : results) {
        backpropagate(chosen_child, playout_winner);
      }
      // Else, just do a single playout:
    } else {
      Cell_state playout_winner = simulate_random_playout(chosen_child, board);
      backpropagate(chosen_child, playout_winner);
    }
    // Print interesting statistics:
    logger->log_root_stats(root->visit_count, root->win_count,
                           root->child_nodes.size());
    for (const auto& child : root->child_nodes) {
      logger->log_child_node_stats(child->move, child->win_count,
                                   child->visit_count);
    }
    mcts_iteration_counter++;
  }
  if (is_verbose) {
    std::cout << "\nTIMER RAN OUT. " << mcts_iteration_counter
              << " iterations completed. CHOOSING A MOVE FROM ROOT'S CHILDREN:"
              << std::endl;
  }
  // Select the child with the highest win ratio as the best move:
  double max_win_ratio = -1.;
  std::shared_ptr<Node> best_child;
  for (const auto& child : root->child_nodes) {
    double win_ratio =
        static_cast<double>(child->win_count) / child->visit_count;
    // If verbose mode is on, print the win ratio for each child node.
    if (is_verbose) {
      std::ostringstream win_ratio_stream;
      if (child->visit_count > 0) {
        win_ratio_stream << std::fixed << std::setprecision(2)
                         << static_cast<double>(child->win_count) /
                                child->visit_count;
      } else {
        win_ratio_stream << "N/A (no visits yet)";
      }

      std::cout << "Child " << child->move.first << "," << child->move.second
                << " has a win ratio of " << win_ratio_stream.str()
                << std::endl;
    }

    if (win_ratio > max_win_ratio) {
      max_win_ratio = win_ratio;
      best_child = child;
    }
  }
  // Would be thrown if the time was too short for a single playout, but
  // sometimes the compiler just ignores the timer. This is a failsafe.
  if (!best_child) {
    throw std::runtime_error(
        "Statistics are not sufficient to choose a move. You likely gave the "
        "robot too little time for the given board size.");
  } else if (is_verbose) {
    std::cout << "\nAfter " << mcts_iteration_counter
              << " iterations, choose child " << best_child->move.first << ", "
              << best_child->move.second << " with win ratio "
              << std::setprecision(4) << max_win_ratio << std::endl;
    std::cout << "\n--------------------MCTS VERBOSE END--------------------\n"
              << std::endl;
  }
  return best_child->move;
}

void Mcts_agent::expand_node(const std::shared_ptr<Node>& node,
                             const Board& board) {
  std::vector<std::pair<int, int>> valid_moves = board.get_valid_moves();
  // For each valid move, create a new child node and add it to the node's
  // children.
  for (const auto& move : valid_moves) {
    std::shared_ptr<Node> new_child =
        std::make_shared<Node>(node->player, move, node);
    node->child_nodes.push_back(new_child);
    if (is_verbose) {
      std::cout << "EXPANDED ROOT'S CHILD: " << move.first << "," << move.second
                << std::endl;
    }
  }
}

double Mcts_agent::calculate_uct_score(
    const std::shared_ptr<Node>& child_node,
    const std::shared_ptr<Node>& parent_node) {
  // If the child node has not been visited yet, return a high value to
  // encourage exploration.
  if (child_node->visit_count == 0) {
    return std::numeric_limits<double>::max();
  } else {
    // Otherwise, calculate the UCT score using the UCT formula.
    return static_cast<double>(child_node->win_count) /
               child_node->visit_count +
           exploration_factor * std::sqrt(std::log(parent_node->visit_count) /
                                          child_node->visit_count);
  }
}

std::shared_ptr<Mcts_agent::Node> Mcts_agent::select_child(
    const std::shared_ptr<Node>& parent_node) {
  // Initialize best_child as the first child and calculate its UCT score
  std::shared_ptr<Node> best_child = parent_node->child_nodes[0];
  double max_score = calculate_uct_score(best_child, parent_node);
  // Iterate over the remaining child nodes to find the one with the highest UCT
  // score
  for (auto iterator = std::next(parent_node->child_nodes.begin());
       iterator != parent_node->child_nodes.end(); ++iterator) {
    const auto& child = *iterator;
    double uct_score = calculate_uct_score(child, parent_node);

    if (uct_score > max_score) {
      max_score = uct_score;
      best_child = child;
    }
  }
  // If verbose mode is enabled, print the move coordinates and UCT score of the
  // selected child
  logger->log_selected_child(best_child->move, max_score);

  return best_child;
}

Cell_state Mcts_agent::simulate_random_playout(
    const std::shared_ptr<Node>& node, Board board) {
  // Start the simulation with the player at the node's move
  Cell_state current_player = node->player;
  board.make_move(node->move.first, node->move.second, current_player);\
  logger->log_simulation_start(node->move, board);
  // Continue simulation until a winner is detected
  while (board.check_winner() == Cell_state::Empty) {
    // Switch player
    current_player = (current_player == Cell_state::Blue) ? Cell_state::Red
                                                          : Cell_state::Blue;
    // Get valid moves and choose one randomly
    std::vector<std::pair<int, int>> valid_moves = board.get_valid_moves();
    // Generate a distribution and choose a move randomly
    std::uniform_int_distribution<> distribution(
        0, static_cast<int>(valid_moves.size() - 1));
    std::pair<int, int> random_move =
        valid_moves[distribution(random_generator)];
    logger->log_simulation_step(current_player, board, random_move);
    board.make_move(random_move.first, random_move.second, current_player);
    // If a player has won, break the loop
    if (board.check_winner() != Cell_state::Empty) {
      logger->log_simulation_end(current_player, board);
      break;
    }
  }
  return current_player;
}

void Mcts_agent::backpropagate(std::shared_ptr<Node>& node, Cell_state winner) {
  // Start backpropagation from the given node
  std::shared_ptr<Node> current_node = node;
  while (current_node != nullptr) {
    // Lock the node's mutex before updating its data
    std::lock_guard<std::mutex> lock(current_node->node_mutex);
    // Increment the node's visit count
    current_node->visit_count += 1;
    // If the winner is the same as the player at the node, increment the node's
    // win count
    if (winner == current_node->player) {
      current_node->win_count += 1;
    }
    logger->log_backpropagation_result(
        current_node->move, current_node->win_count, current_node->visit_count);
    // Move to the parent node for the next iteration
    current_node = current_node->parent_node;
  }
}
