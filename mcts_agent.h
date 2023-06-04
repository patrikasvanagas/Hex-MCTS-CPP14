#ifndef MCTS_AGENT_H
#define MCTS_AGENT_H

#include <chrono>
#include <memory>
#include <mutex>
#include <random>
#include <vector>

#include "board.h"
#include "logger.h"

class Mcts_agent {
 public:
  Mcts_agent(double exploration_factor,
             std::chrono::milliseconds max_decision_time, bool is_parallelized,
             bool is_verbose = false);

  std::pair<int, int> choose_move(const Board& board, Cell_state player);

 private:
  double exploration_factor;
  std::chrono::milliseconds max_decision_time;
  bool is_parallelized = false;
  bool is_verbose = false;

  std::random_device random_device;
  std::mt19937 random_generator;

  std::shared_ptr<Logger> logger;

  struct Node;
  std::shared_ptr<Node> root;

  struct Node {
    int win_count;
    int visit_count;
    std::pair<int, int> move;
    Cell_state player;
    std::vector<std::shared_ptr<Node>> child_nodes;
    std::shared_ptr<Node> parent_node;
    std::mutex node_mutex;
    Node(Cell_state player, std::pair<int, int> move,
         std::shared_ptr<Node> parent_node = nullptr);
  };

  void expand_node(const std::shared_ptr<Node>& node, const Board& board);
  void perform_mcts_iterations(
      const std::chrono::time_point<std::chrono::high_resolution_clock>&
          end_time,
      int& mcts_iteration_counter, const Board& board,
      unsigned int number_of_threads);
  double calculate_uct_score(const std::shared_ptr<Node>& child_node,
                             const std::shared_ptr<Node>& parent_node);
  std::shared_ptr<Node> select_child(const std::shared_ptr<Node>& parent_node);
  Cell_state simulate_random_playout(const std::shared_ptr<Node>& node,
                                     Board board);
  std::vector<Cell_state> parallel_playout(std::shared_ptr<Node> node,
                                           const Board& board,
                                           unsigned int number_of_threads);
  void backpropagate(std::shared_ptr<Node>& node, Cell_state winner);
  std::shared_ptr<Node> select_best_child();
};

#endif
