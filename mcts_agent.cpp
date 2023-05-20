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

/**
 * @brief Constructs a new Mcts_agent.
 *
 * @param exploration_factor Determines the constant of exploration in
 * the UCT formula.
 * @param max_decision_time Maximum time allowed for making a decision
 * in milliseconds.
 * @param is_parallelized Determines if playouts are performed in parallel.
 * Sufficient time has to be given for this to be effective.
 * @param is_verbose If true, enables detailed logging to the console.
 * 
 * @throws std::logic_error if is_parallelized and is_verbose are both true.
 * This is because the output would be garbled.
 */
Mcts_agent::Mcts_agent(double exploration_factor,
                       std::chrono::milliseconds max_decision_time,
                       bool is_parallelized, bool is_verbose)
    : exploration_factor(exploration_factor),
      max_decision_time(max_decision_time),
      is_verbose(is_verbose),
      random_generator(random_device()) 
{
  if (is_parallelized && is_verbose) {
    throw std::logic_error(
        "Concurrent playouts and verbose mode do not make sense together.");
  }
}

// Constructor for Node
/**
 * @brief Constructs a new Node.
 *
 * @param player The player making a move.
 * @param move The move made by the player.
 * @param parent_node The parent node in the tree (default: nullptr).
 */
Mcts_agent::Node::Node(Cell_state player, std::pair<int, int> move,
                       std::shared_ptr<Node> parent_node)
    : win_count(0),
      visit_count(0),
      move(move),
      player(player),
      child_nodes(),
      parent_node(parent_node) 
{
}

/**
 * Chooses the best move for a given game state using the Monte Carlo Tree
 * Search (MCTS) algorithm.
 *
 * This function uses the MCTS algorithm to explore the game tree and identify
 * the best move for the given game state. The algorithm simulates numerous
 * games starting at the current state, then uses the results of these
 * simulations to make a decision.
 *
 * The function creates a new root node for the MCTS, then expands this node
 * based on the current game state. It then enters a loop in which it selects a
 * child node, simulates a game from this node, and backpropagates the result of
 * the game back up the tree. This loop continues until the allocated
 * decision-making time is exhausted.
 *
 * After the loop, the function chooses the child of the root node with the
 * highest win ratio as the best move. If verbose mode is active, it also prints
 * various statistics about the MCTS process.
 *
 * Note: The function can work in both a single-threaded and a multi-threaded
 * mode. The latter is activated by setting `is_parallelized` to `true`.
 *
 * @param board The current game state.
 * @param player The player for whom the move is being chosen.
 * @return A std::pair<int, int> representing the best move for the given player
 * in the current game state.
 * @throws runtime_error If the statistics are not sufficient to choose a move.
 * This can happen if the robot was given too little time for the given board
 * size.
 */
std::pair<int, int> Mcts_agent::choose_move(const Board& board,
                                            Cell_state player) 
{
  if (is_verbose) {
    std::cout << "\n-------------MCTS VERBOSE START - " << player
              << " to move-------------\n"
              << std::endl;
  }
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
  //Expand root based on the current game state
  expand_node(root, board);
  int mcts_iteration_counter = 0;
  auto start_time = std::chrono::high_resolution_clock::now();
  auto end_time = start_time + max_decision_time;
  // The main loop of the MCTS. It runs until the decision-making time limit is
  // reached.
  while (std::chrono::high_resolution_clock::now() < end_time) {
    if (is_verbose) {
      std::cout << "\n------------------STARTING SIMULATION "
                << mcts_iteration_counter + 1 << "------------------"
                << std::endl;
    }
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
    if (is_verbose) {
      std::cout << "\nAFTER BACKPROPAGATION, root node has "
                << root->visit_count << " visits, " << root->win_count
                << " wins, and " << root->child_nodes.size()
                << " child nodes. Their details are:\n";
      for (const auto& child : root->child_nodes) {
        std::ostringstream node_info_stream;
        node_info_stream << "Child node " << child->move.first << ","
                         << child->move.second << ": Wins: " << child->win_count
                         << ", Visits: " << child->visit_count
                         << ". Win ratio: ";

        if (child->visit_count) {
          node_info_stream << std::fixed << std::setprecision(2)
                           << static_cast<double>(child->win_count) /
                                  child->visit_count;
        } else {
          node_info_stream << "N/A (no visits yet)";
        }

        std::cout << node_info_stream.str() << std::endl;
      }
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
  //Would be thrown if the time was too short for a single playout, but
  //sometimes the compiler just ignores the timer. This is a failsafe.
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

/**
 * @brief Expands a given node by generating all its possible child nodes based
 * on the valid moves on the current game board.
 *
 * This function populates the `child_nodes` member of the input `Node` with new
 * nodes, each representing a valid move for the player at the current game
 * state. Each child node is linked back to the input node as its parent.
 *
 * If verbose mode is enabled, the function will also print information about
 * each new child node it creates.
 *
 * @param node A shared_ptr to the Node to be expanded.
 * @param board The current game state.
 */
void Mcts_agent::expand_node(const std::shared_ptr<Node>& node,
                             const Board& board) 
{
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

/**
 * @brief Calculates the Upper Confidence Bound for Trees (UCT) score for a
 * given node.
 *
 * The UCT score is used in the selection phase of the Monte Carlo Tree Search
 * (MCTS) algorithm to balance the exploration and exploitation trade-off. The
 * function uses the UCT formula, which is a sum of the exploitation term (win
 * ratio) and the exploration term. The exploration term is proportional to the
 * square root of the logarithm of the parent node's visit count divided by the
 * child node's visit count.
 *
 * The function returns a high value if the child node has not been visited yet,
 * to encourage the exploration of unvisited nodes.
 *
 * @param child_node A shared_ptr to the child Node for which the UCT score is
 * being calculated.
 * @param parent_node A shared_ptr to the parent Node of the child node.
 * @return The calculated UCT score.
 */
double Mcts_agent::calculate_uct_score(
    const std::shared_ptr<Node>& child_node,
    const std::shared_ptr<Node>& parent_node) 
{
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

/**
 * @brief Selects the best child of a given parent node based on the Upper
 * Confidence Bound for Trees (UCT) score.
 *
 * This function iterates through all the child nodes of the given parent node,
 * and for each child, calculates its UCT score using the calculate_uct_score()
 * method. The child with the highest UCT score is selected as the best child.
 * If verbose mode is enabled, the function prints the move coordinates and the
 * UCT score of the selected child.
 *
 * @param parent_node A shared_ptr to the parent Node whose child nodes are to
 * be evaluated.
 * @return A shared_ptr to the Node that is selected as the best child.
 */
std::shared_ptr<Mcts_agent::Node> Mcts_agent::select_child(
    const std::shared_ptr<Node>& parent_node) 
{
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
  if (is_verbose) {
    std::cout << "\nSELECTED CHILD " << best_child->move.first << ", "
              << best_child->move.second << " with UCT of ";
    if (max_score == std::numeric_limits<double>::max()) {
      std::cout << "infinity" << std::endl;
    } else {
      std::cout << std::setprecision(4) << max_score << std::endl;
    }
  }

  return best_child;
}

/**
 * @brief Simulates a random playout from a given node on a given board.
 *
 * This function takes as input a node and a board state, and simulates a random
 * playout starting from the node's move. The simulation proceeds by alternating
 * between players, choosing a random valid move for each player, until the game
 * ends (i.e., when a player wins). If verbose mode is enabled, the function
 * also prints information about the simulation, including the move made
 * at each step and the state of the board and its state.
 *
 * @param node A shared_ptr to the Node from which the simulation starts.
 * @param board The Board on which the simulation is conducted. The board state
 * is copied, so the original board is not modified.
 * @return The Cell_state of the winning player.
 */
Cell_state Mcts_agent::simulate_random_playout(
    const std::shared_ptr<Node>& node, Board board) 
{
  // Start the simulation with the player at the node's move
  Cell_state current_player = node->player;
  board.make_move(node->move.first, node->move.second, current_player);
  if (is_verbose) {
    std::cout << "\nSIMULATING A RANDOM PLAYOUT from node " << node->move.first
              << ", " << node->move.second
              << ". Simulation board is in state:\n"
              << board;
  }
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
    if (is_verbose) {
      std::cout << "Current player in simulation is " << current_player
                << " in Board state:\n"
                << board;
      std::cout << current_player << " makes random move " << random_move.first
                << "," << random_move.second << ". ";
    }

    board.make_move(random_move.first, random_move.second, current_player);
    // If a player has won, break the loop
    if (board.check_winner() != Cell_state::Empty) {
      if (is_verbose) {
        std::cout << "DETECTED WIN for player " << current_player
                  << " in Board state:\n"
                  << board << "\n";
      }

      break; 
    }
  }

  return current_player;
}

/**
 * @brief Backpropagates the result of a simulation through the tree.
 *
 * This function takes a node and the winner of a game simulation, and
 * backpropagates the result through the tree. It starts at the given node and
 * moves up towards the root, incrementing the visit count of each node along
 * the way. If the winner is the same as the player at a node, it also
 * increments the win count of that node. The process continues until the root
 * is reached. The function is designed to be thread-safe by locking the node's
 * mutex before updating its data.
 *
 * @note In the current implementation, the function only traverses two levels
 * of the tree (from a child node to its root), but it can be used to
 * traverse the entire tree if needed.
 *
 * @param node A shared_ptr to the Node at which to start the backpropagation.
 * @param winner The Cell_state of the winning player in the game simulation.
 */
void Mcts_agent::backpropagate(std::shared_ptr<Node>& node, Cell_state winner) 
{
  // Start backpropagation from the given node
  std::shared_ptr<Node> current_node = node;
  while (current_node != nullptr) {
    // Lock the node's mutex before updating its data
    std::lock_guard<std::mutex> lock(
        current_node->node_mutex);
    // Increment the node's visit count
    current_node->visit_count += 1;
    // If the winner is the same as the player at the node, increment the node's
    // win count
    if (winner == current_node->player) {
      current_node->win_count += 1;
    }

    if (is_verbose) {
      std::cout << "BACKPROPAGATED result to node " << current_node->move.first
                << ", " << current_node->move.second << ". It currently has "
                << current_node->win_count << " wins and "
                << current_node->visit_count << " visits." << std::endl;
    }
    // Move to the parent node for the next iteration
    current_node = current_node->parent_node;
  }
}
