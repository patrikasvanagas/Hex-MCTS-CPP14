#ifndef MCTS_AGENT_H
#define MCTS_AGENT_H

#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include "hex_board.h"

class MCTSAgent {
public:
    MCTSAgent(double exploration_constant, std::chrono::milliseconds move_time_limit, bool verbose = false);
    std::pair<int, int> choose_move(const Board& board, char player);
private:
    double exploration_constant;
    std::chrono::milliseconds move_time_limit;
    bool verbose = false;
    std::random_device rd;
    std::mt19937 gen;
    struct Node;
    std::shared_ptr<Node> root;
    struct Node {
        int wins;
        int visits;
        std::pair<int, int> move;
        char player;
        std::vector<std::shared_ptr<Node>> children;
        std::shared_ptr<Node> parent;
        Node(char player, std::pair<int, int> move, std::shared_ptr<Node> parent = nullptr);
    };
    char get_opponent(char player) const;
    std::shared_ptr<Node> select_child(const std::shared_ptr<Node>& node);
    void expand_root(const std::shared_ptr<Node>& node, const Board& board);
    char simulate_random_playout(const std::shared_ptr<Node>& node, Board board);
    void backpropagate(std::shared_ptr<Node>& node, char winner);
};

#endif