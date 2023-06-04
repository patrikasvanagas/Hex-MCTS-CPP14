#ifndef MCTS_AGENT_H
#define MCTS_AGENT_H

#include <chrono>
#include <memory>
#include <mutex>
#include <random>
#include <vector>

#include "board.h"
#include "logger.h"

/**
 * @brief This class provides the agent for Monte Carlo Tree Search (MCTS).
 * It is used to make strategic decisions in a board game by simulating
 * possible moves and choosing the most promising one.
 * Node is a nested class that represents a node in the MCTS tree.
 */
class Mcts_agent {
 public:
  /**
   * @brief Constructs a new Mcts_agent.
   *
   * @param exploration_factor Determines the constant of exploration in the
   * UCT formula.
   * @param max_decision_time Maximum time allowed for making a decision
   * in milliseconds.
   * @param is_parallelized Determines if playouts are performed in
   * parallel Sufficient time has to be given for this to be effective.
   * @param is_verbose If true, enables detailed logging to the console
   * using the Logger class.
   */
  Mcts_agent(double exploration_factor,
             std::chrono::milliseconds max_decision_time, bool is_parallelized,
             bool is_verbose = false);

  /**
   * @brief Chooses a move using the MCTS algorithm.
   *
   * This function chooses a move by performing MCTS iterations until the
   * maximum decision time is reached. It then selects the most promising
   * move and returns it.
   *
   * @param board The current game board.
   * @param player The player for which the move is chosen.
   * @return The chosen move.
   */
  std::pair<int, int> choose_move(const Board& board, Cell_state player);

 private:
  // Agent configuration parameters
  double exploration_factor;
  std::chrono::milliseconds max_decision_time;
  bool is_parallelized = false;
  bool is_verbose = false;

  // For random number generation
  std::random_device random_device;
  std::mt19937 random_generator;

  // For logging
  std::shared_ptr<Logger> logger;

  // The root node of the game tree
  struct Node;
  std::shared_ptr<Node> root;

  /**
   * @brief A nested structure representing a node in the search tree for Monte
   * Carlo Tree Search (MCTS).
   *
   * Each node in the tree corresponds to a unique game state.
   * It contains information about the game state and also about the progress of
   * the search.
   */
  struct Node {
    /**
     * @brief The number of wins that have been recorded through this node.
     *
     * This is incremented every time a simulation (or playout) that passes
     * through this node results in a win.
     */
    int win_count;
    /**
     * @brief The number of times this node has been visited during the search.
     *
     * This is incremented every time the search algorithm selects this node to
     * start a new simulation (or playout).
     */
    int visit_count;
    /**
     * @brief The move that led to this game state from the parent node's game
     * state. For a parent node, it's filler value is (-1, -1).
     *
     * This is represented as a pair of integers, typically representing
     * coordinates on a game board.
     */
    std::pair<int, int> move;
    /**
     * @brief The player who made the move from the parent node's state to this
     * node's state (Cell_state).
     *
     * This indicates who will make the next move in the game state associated
     * with this node.
     */
    Cell_state player;
    /**
     * @brief The child nodes of this node, each representing a game state that
     * can be reached from this node's game state by one move.
     */
    std::vector<std::shared_ptr<Node>> child_nodes;
    /**
     * @brief A pointer to the parent node of this node, representing the game
     * state from which this node's game state can be reached by one move.
     */
    std::shared_ptr<Node> parent_node;
    /**
     * @brief A mutex to ensure thread-safety during the updating of the node's
     * data.
     *
     * This is especially important when the MCTS algorithm is used with
     * parallel computations.
     */
    std::mutex node_mutex;

    /**
     * Constructs a new Node.
     *
     * @param player The player making a move (Cell_state).
     * @param move The move that can be made by the player. (-1, -1) if
     * the node is the root node.
     * @param parent_node The parent node in the tree. default: nullptr
     * is used for the root node.
     */
    Node(Cell_state player, std::pair<int, int> move,
         std::shared_ptr<Node> parent_node = nullptr);
  };

  // Private helper functions for MCTS. More extensive docs are in
  // mcts_agent.cpp.

  /**
   * @brief Expands a node in the game tree.
   *
   * This function generates all possible child nodes for a given node.
   *
   * @param node The node to be expanded.
   * @param board The current game state.
   */
  void expand_node(const std::shared_ptr<Node>& node, const Board& board);

  /**
   * @brief Executes Monte Carlo Tree Search (MCTS) iterations until a set end
   * time.
   *
   * This function performs selection, simulation, backpropagation and logging
   * in each MCTS iteration. It supports both serial and parallel playouts.
   *
   * @param end_time Time point to stop performing iterations.
   * @param mcts_iteration_counter Counter to keep track of the number of MCTS
   * iterations.
   * @param board Current state of the game board.
   * @param number_of_threads Number of threads for parallel playouts (only
   * applicable if is_parallelized is true).
   */
  void perform_mcts_iterations(
      const std::chrono::time_point<std::chrono::high_resolution_clock>&
          end_time,
      int& mcts_iteration_counter, const Board& board,
      unsigned int number_of_threads);

  /**
   * @brief Selects a child node of a given parent node for the playout phase.
   *
   * This method chooses a child node based on the Upper Confidence Bound for
   * Trees (UCT) score, which balances exploration and exploitation.
   *
   * @param parent_node The parent Node from which a child node is selected.
   * @return A pointer to the selected child Node.
   */
  std::shared_ptr<Node> select_child_for_playout(
      const std::shared_ptr<Node>& parent_node);

  double calculate_uct_score(const std::shared_ptr<Node>& child_node,
                             const std::shared_ptr<Node>& parent_node);

  Cell_state simulate_random_playout(const std::shared_ptr<Node>& node,
                                     Board board);
  std::vector<Cell_state> parallel_playout(std::shared_ptr<Node> node,
                                           const Board& board,
                                           unsigned int number_of_threads);
  void backpropagate(std::shared_ptr<Node>& node, Cell_state winner);
  std::shared_ptr<Node> select_best_child();
};

#endif
