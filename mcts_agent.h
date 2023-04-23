#ifndef MCTS_AGENT_H
#define MCTS_AGENT_H

#include <vector>
#include <memory>
#include <chrono>
#include "board.h"

class MCTSAgent {
public:
    MCTSAgent(double exploration_constant, std::chrono::milliseconds move_time_limit);
    std::pair<int, int> choose_move(const Board& board, char player);

private:
    double exploration_constant;
    std::chrono::milliseconds move_time_limit;
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

    std::shared_ptr<Node> select_node(const std::shared_ptr<Node>& node, const Board& board);
    std::shared_ptr<Node> expand_node(const std::shared_ptr<Node>& node, Board& board);
    void simulate_random_playout(Board& board, char current_player);
    void backpropagate(const std::shared_ptr<Node>& node, char winner);
};

#endif
