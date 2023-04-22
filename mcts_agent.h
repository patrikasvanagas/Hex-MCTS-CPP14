#ifndef MCTS_AGENT_H
#define MCTS_AGENT_H

#include <vector>
#include <memory>
#include "board.h"

class MCTSAgent {
public:
    MCTSAgent(int simulations);
    std::pair<int, int> choose_move(const Board& board, char player);

private:
    int num_simulations;
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

    std::shared_ptr<Node> select(const std::shared_ptr<Node>& node, Board board);
    void expand(const std::shared_ptr<Node>& node, const Board& board);
    void simulate(const std::shared_ptr<Node>& node, Board board);
    void backpropagate(const std::shared_ptr<Node>& node, char winner);
};

#endif
