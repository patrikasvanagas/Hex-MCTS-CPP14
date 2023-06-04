#include "mcts_agent.h"

#include <cassert>
#include <cmath>
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
  unsigned int number_of_threads = 1;
  if (is_parallelized) {
    // Determine the maximum number of threads available on the hardware.
    number_of_threads = std::thread::hardware_concurrency();
  }
  // Expand root based on the current game state
  expand_node(root, board);
  int mcts_iteration_counter = 0;
  auto start_time = std::chrono::high_resolution_clock::now();
  auto end_time = start_time + max_decision_time;
  perform_mcts_iterations(end_time, mcts_iteration_counter, board,
                          number_of_threads);
  logger->log_timer_ran_out(mcts_iteration_counter);
  // Select the child with the highest win ratio as the best move:
  std::shared_ptr<Node> best_child = select_best_child();
  logger->log_best_child_chosen(
      mcts_iteration_counter, best_child->move,
      static_cast<double>(best_child->win_count) / best_child->visit_count);
  logger->log_mcts_end();
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
    logger->log_expanded_child(move);
  }
}

void Mcts_agent::perform_mcts_iterations(
    const std::chrono::time_point<std::chrono::high_resolution_clock>& end_time,
    int& mcts_iteration_counter, const Board& board,
    unsigned int number_of_threads) {
  while (std::chrono::high_resolution_clock::now() < end_time) {
    logger->log_iteration_number(mcts_iteration_counter + 1);
    std::shared_ptr<Node> chosen_child = select_child(root);
    if (is_parallelized) {
      std::vector<Cell_state> results =
          parallel_playout(chosen_child, board, number_of_threads);
      // Backpropagate each of the results
      for (Cell_state playout_winner : results) {
        backpropagate(chosen_child, playout_winner);
      }
      // Else, just do a single playout:
    } else {
      Cell_state playout_winner = simulate_random_playout(chosen_child, board);
      backpropagate(chosen_child, playout_winner);
    }
    // Print statistics:
    logger->log_root_stats(root->visit_count, root->win_count,
                           root->child_nodes.size());
    for (const auto& child : root->child_nodes) {
      logger->log_child_node_stats(child->move, child->win_count,
                                   child->visit_count);
    }
    mcts_iteration_counter++;
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

Cell_state Mcts_agent::simulate_random_playout(
    const std::shared_ptr<Node>& node, Board board) {
  // Start the simulation with the player at the node's move
  Cell_state current_player = node->player;
  board.make_move(node->move.first, node->move.second, current_player);
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

std::vector<Cell_state> Mcts_agent::parallel_playout(
    std::shared_ptr<Node> node, const Board& board,
    unsigned int number_of_threads) {
  std::vector<std::thread> threads;
  std::vector<Cell_state> results(number_of_threads);

  // Start the threads and put their separate results into a vector
  for (unsigned int thread_index = 0; thread_index < number_of_threads;
       thread_index++) {
    threads.push_back(std::thread([&, thread_index]() {
      results[thread_index] = simulate_random_playout(node, board);
    }));
  }

  // Join the threads
  for (auto& thread : threads) {
    thread.join();
  }

  return results;
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

std::shared_ptr<Mcts_agent::Node> Mcts_agent::select_best_child() {
  double max_win_ratio = -1.;
  std::shared_ptr<Node> best_child;
  for (const auto& child : root->child_nodes) {
    double win_ratio =
        static_cast<double>(child->win_count) / child->visit_count;
    // If verbose mode is on, print the win ratio for each child node.
    logger->log_node_win_ratio(child->move, child->win_count,
                               child->visit_count);
    if (win_ratio > max_win_ratio) {
      max_win_ratio = win_ratio;
      best_child = child;
    }
  }
  if (!best_child) {
    throw std::runtime_error(
        "Statistics are not sufficient to choose a move. You likely gave the "
        "robot too little time for the given board size.");
  }
  return best_child;
}