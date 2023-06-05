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
 * @class Mcts_agent
 *
 * @brief This class implements a Monte Carlo Tree Search (MCTS) agent to choose
 * moves in a game.
 *
 * The `Mcts_agent` class uses MCTS to simulate game play and choose the best
 * move for a given game state. It supports optional parallelization for
 * increased performance, and offers optional verbosity for logging purposes.
 *
 * MCTS works by simulating the game from the current state to a terminal state,
 * then updating the statistics of each visited node based on the outcome. This
 * is done repeatedly until a pre-set time limit is reached. The agent then
 * chooses the move that leads to the node with the highest win ratio.
 *
 * @note This class assumes a game interface with `Board` and `Cell_state` types
 * defined, and a `Logger` class for logging purposes. The `Board` class should
 * have methods `get_valid_moves()` to return a list of valid moves,
 * `make_move()` to make a move, and `check_winner()` to check if there is a
 * winner. `Cell_state` should define the state of a cell on the board, and
 * should at least include `Empty`, `Red`, and `Blue`.
 *
 * @param exploration_factor A parameter to balance exploration vs exploitation
 * in the UCT formula.
 * @param max_decision_time The maximum time in milliseconds that the agent can
 * use to make a decision.
 * @param is_parallelized Determines whether the MCTS iterations should be
 * parallelized.
 * @param is_verbose If true, the agent logs more detailed information about its
 * decision-making process.
 *
 */
class Mcts_agent {
 public:
  /**
   * @brief Constructs a new Mcts_agent.
   *
   * @param exploration_factor Determines the constant of exploration in
   * the UCT formula.
   * @param max_decision_time Maximum time allowed for making a decision
   * in milliseconds.
   * @param is_parallelized Determines if playouts are performed in parallel.
   * Sufficient time has to be given for this to be effective.
   * @param is_verbose If true, enables detailed logging to the console
   * using the Logger class.
   *
   * @throws std::logic_error if is_parallelized and is_verbose are both true.
   * This is because the output would be garbled.
   */
  Mcts_agent(double exploration_factor,
             std::chrono::milliseconds max_decision_time, bool is_parallelized,
             bool is_verbose = false);

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
   * based on the current game state. It then enters a loop in which it selects
   * a child node, simulates a game from this node, and backpropagates the
   * result of the game back up the tree. This loop continues until the
   * allocated decision-making time is exhausted.
   *
   * After the loop, the function chooses the child of the root node with the
   * highest win ratio as the best move. If verbose mode is active, it also
   * prints various statistics about the MCTS process using Logger.
   *
   * Note: The function can work in both a single-threaded and a multi-threaded
   * mode. The latter is activated by setting `is_parallelized` to `true`.
   *
   * @param board The current game state.
   * @param player The player for whom the move is being chosen.
   * @return A std::pair<int, int> representing the best move for the given
   * player in the current game state.
   * @throws runtime_error If the statistics are not sufficient to choose a
   * move. This can happen if the robot was given too little time for the given
   * board size.
   */
  std::pair<int, int> choose_move(const Board& board, Cell_state player);

 private:
  // Agent configuration parameters
  double exploration_factor;
  std::chrono::milliseconds max_decision_time;
  bool is_parallelized = false;
  bool is_verbose = false;

  // For logging
  std::shared_ptr<Logger> logger;

  // For random number generation
  std::random_device random_device;
  std::mt19937 random_generator;

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

  /**
   * @brief Expands a given node by generating all its possible child nodes
   * based on the valid moves on the current game board.
   *
   * This function populates the `child_nodes` member of the input `Node` with
   * new nodes, each representing a valid move for the player at the current
   * game state. Each child node is linked back to the input node as its parent.
   *
   * If verbose mode is enabled, the function will also print information about
   * each new child node it creates.
   *
   * @param node A shared_ptr to the Node to be expanded.
   * @param board The current game state.
   */
  void expand_node(const std::shared_ptr<Node>& node, const Board& board);

  /**
   * @brief Performs the main loop of the Monte Carlo Tree Search (MCTS)
   * algorithm.
   *
   * This function performs multiple iterations of the MCTS algorithm until a
   * provided end time is reached. In each iteration, a child node is selected
   * from the root node using the UCT score, and a playout is simulated from
   * this node, either in parallel or serially depending on the value of
   * `is_parallelized`. The results of the playout are then backpropagated up
   * the MCTS tree. The function also logs various statistics of the root node
   * and its children after each iteration using the Logger class.
   *
   * @param end_time The end time for the MCTS iterations. The function will
   * continue performing iterations until the current time is greater than this
   * value.
   * @param mcts_iteration_counter A reference to an integer counter for the
   * number of MCTS iterations performed so far. This counter is incremented
   * after each iteration.
   * @param board The current state of the game board.
   * @param number_of_threads The number of threads to use for parallel
   * playouts. This parameter is only relevant if `is_parallelized` is true.
   */
  void perform_mcts_iterations(
      const std::chrono::time_point<std::chrono::high_resolution_clock>&
          end_time,
      int& mcts_iteration_counter, const Board& board,
      unsigned int number_of_threads);

  /**
   * @brief Selects the best child of a given parent node based on the Upper
   * Confidence Bound for Trees (UCT) score.
   *
   * This function iterates through all the child nodes of the given parent
   * node, and for each child, calculates its UCT score using the
   * calculate_uct_score() method. The child with the highest UCT score is
   * selected as the best child. If verbose mode is enabled, the function prints
   * the move coordinates and the UCT score of the selected child.
   *
   * @param parent_node A shared_ptr to the parent Node whose child nodes are to
   * be evaluated.
   * @return A shared_ptr to the Node that is selected as the best child.
   */
  std::shared_ptr<Node> select_child_for_playout(
      const std::shared_ptr<Node>& parent_node);

  /**
   * @brief Calculates the Upper Confidence Bound for Trees (UCT) score for a
   * given node.
   *
   * The UCT score is used in the selection phase of the Monte Carlo Tree Search
   * (MCTS) algorithm to balance the exploration and exploitation trade-off. The
   * function uses the UCT formula, which is a sum of the exploitation term (win
   * ratio) and the exploration term. The exploration term is proportional to
   * the square root of the logarithm of the parent node's visit count divided
   * by the child node's visit count.
   *
   * The function returns a high value if the child node has not been visited
   * yet, to encourage the exploration of unvisited nodes.
   *
   * @param child_node A shared_ptr to the child Node for which the UCT score is
   * being calculated.
   * @param parent_node A shared_ptr to the parent Node of the child node.
   * @return The calculated UCT score.
   */
  double calculate_uct_score(const std::shared_ptr<Node>& child_node,
                             const std::shared_ptr<Node>& parent_node);

  /**
   * @brief Simulates a random playout from a given node on a given board.
   *
   * This function takes as input a node and a board state, and simulates a
   * random playout starting from the node's move. The simulation proceeds by
   * alternating between players, choosing a random valid move for each player,
   * until the game ends (i.e., when a player wins). If verbose mode is enabled,
   * the function also prints information about the simulation, including the
   * move made at each step and the state of the board and its state using
   * Logger.
   *
   * @param node A shared_ptr to the Node from which the simulation starts.
   * @param board The Board on which the simulation is conducted. The board
   * state is copied, so the original board is not modified.
   * @return The Cell_state of the winning player.
   */
  Cell_state simulate_random_playout(const std::shared_ptr<Node>& node,
                                     Board board);

  /**
   * @brief Performs a number of game playouts in parallel from a given node and
   * returns their results.
   *
   * This function simulates several game playouts starting from a given node in
   * parallel using multiple threads. It returns the outcome of each playout in
   * a vector, with the result of the playout simulated by the i-th thread
   * stored in the i-th position of the vector.
   *
   * @param node The node from which the playouts should be simulated.
   * @param board The current state of the game board.
   * @param number_of_threads The number of threads to use for the parallel
   * playouts.
   * @return A vector of Cell_state, where each element is the the playout
   * winner.
   */
  std::vector<Cell_state> parallel_playout(std::shared_ptr<Node> node,
                                           const Board& board,
                                           unsigned int number_of_threads);

  /**
   * @brief Backpropagates the result of a simulation through the tree.
   *
   * This function takes a node and the winner of a game simulation, and
   * backpropagates the result through the tree. It starts at the given node and
   * moves up towards the root, incrementing the visit count of each node along
   * the way. If the winner is the same as the player at a node, it also
   * increments the win count of that node. The process continues until the root
   * is reached. The function is designed to be thread-safe by locking the
   * node's mutex before updating its data.
   *
   * @note In the current implementation, the function only traverses two levels
   * of the tree (from a child node to its root), but it can be used to
   * traverse the entire tree if needed.
   *
   * @param node A shared_ptr to the Node at which to start the backpropagation.
   * @param winner The Cell_state of the winning player in the game simulation.
   */
  void backpropagate(std::shared_ptr<Node>& node, Cell_state winner);

  /**
   * @brief Selects the best child of the root node based on the highest win
   * ratio.
   *
   * This function iterates over the root's child nodes, calculates the win
   * ratio for each child node (i.e., the child node's win count divided by its
   * visit count), and returns the child node with the highest win ratio. If
   * verbose mode is enabled, it logs the win ratio for each child node. If no
   * child node can be selected due to insufficient statistics (which might
   * occur if the agent was given too little decision time for the board size),
   * it throws a runtime error.
   *
   * @return A shared pointer to the child node with the highest win ratio.
   * @throws std::runtime_error If no child can be selected due to insufficient
   * statistics.
   */
  std::shared_ptr<Node> select_best_child();
};

#endif
