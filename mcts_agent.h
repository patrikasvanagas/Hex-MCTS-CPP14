#ifndef MCTS_AGENT_H
#define MCTS_AGENT_H

#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include <mutex>
#include "board.h"

class Mcts_agent {
public:
    Mcts_agent(double exploration_factor, std::chrono::milliseconds max_decision_time, bool is_parallelized, bool is_verbose = false);
    std::pair<int, int> choose_move(const Board& board, char player);
private:
    double exploration_factor;
    std::chrono::milliseconds max_decision_time;
    bool is_parallelized = false;
    bool is_verbose = false;
    std::random_device random_device;
    std::mt19937 random_generator;
    struct Node;
    std::shared_ptr<Node> root;
    struct Node {
        int win_count;
        int visit_count;
        std::pair<int, int> move;
        char player;
        std::vector<std::shared_ptr<Node>> child_nodes;
        std::shared_ptr<Node> parent_node;
        std::mutex node_mutex;
        Node(char player, std::pair<int, int> move, std::shared_ptr<Node> parent_node = nullptr);
    };
    char get_opponent(char current_player) const;
    std::shared_ptr<Node> select_child(const std::shared_ptr<Node>& parent_node);
    void expand_node(const std::shared_ptr<Node>& node, const Board& board);
    char simulate_random_playout(const std::shared_ptr<Node>& node, Board board);
    void backpropagate(std::shared_ptr<Node>& node, char winner);
};

#endif