#include <limits>
#include <memory>
#include <cmath>
#include <random>
#include "mcts_agent.h"

MCTSAgent::MCTSAgent(int simulations) : num_simulations(simulations) {}

MCTSAgent::Node::Node(char player, std::pair<int, int> move, std::shared_ptr<Node> parent)
    : wins(0), visits(0), move(move), player(player), children(), parent(parent) {}

std::pair<int, int> MCTSAgent::choose_move(const Board& board, char player) {
    root = std::make_shared<Node>(player, std::make_pair(-1, -1), nullptr);

    for (int i = 0; i < num_simulations; ++i) {
        std::shared_ptr<Node> node = select(root, board);
        expand(node, board);
        Board simulated_board(board); // Create a copy of the board for simulation
        simulate(node, simulated_board); // Run the simulation
        char winner = simulated_board.check_winner(); // Check the winner of the simulated board
        backpropagate(node, winner);
    }

    int max_visits = -1;
    std::shared_ptr<Node> best_child;
    for (const auto& child : root->children) {
        if (child->visits > max_visits) {
            max_visits = child->visits;
            best_child = child;
        }
    }

    return best_child->move;
}

std::shared_ptr<MCTSAgent::Node> MCTSAgent::select(const std::shared_ptr<Node>& node, Board board) {
    // Check if the node represents a terminal game state (i.e., a win or draw).
    char winner = board.check_winner();
    // Return the node if it's a leaf node (i.e., it has no children and has been visited at least once)
    // or if it represents a terminal game state.
    if ((node->children.empty() && node->visits > 0) || winner != '.') {
        return node;
    }
    double max_score = std::numeric_limits<double>::lowest();
    std::shared_ptr<Node> best_child;
    for (const auto& child : node->children) {
        double uct_score = static_cast<double>(child->wins) / child->visits + std::sqrt(2 * std::log(node->visits) / child->visits);
        if (uct_score > max_score) {
            max_score = uct_score;
            best_child = child;
        }
    }
    // Update the board state before making the recursive call.
    if (best_child != nullptr) {
        board.make_move(best_child->move.first, best_child->move.second, node->player);
        return select(best_child, board);
    }
    else {
        return node;
    }
}




void MCTSAgent::expand(const std::shared_ptr<Node>& node, const Board& board) {
    if (node->visits > 0) {
        for (int x = 0; x < board.get_board_size(); ++x) {
            for (int y = 0; y < board.get_board_size(); ++y) {
                if (board.is_valid_move(x, y)) {
                    char next_player = (node->player == 'B') ? 'W' : 'B';
                    node->children.push_back(std::make_shared<Node>(next_player, std::make_pair(x, y), node));
                }
            }
        }
    }

    // If the node still has no children after the expansion attempt, it's a leaf node.
    // Run the simulation for the leaf node.
    if (node->children.empty()) {
        Board simulated_board(board);
        simulate(node, simulated_board);
    }

    node->visits++;
}


void MCTSAgent::simulate(const std::shared_ptr<Node>& node, Board board) {
    std::random_device rd;
    std::mt19937 gen(rd());

    while (board.check_winner() == '.') {
        std::vector<std::pair<int, int>> valid_moves;
        for (int x = 0; x < board.get_board_size(); ++x) {
            for (int y = 0; y < board.get_board_size(); ++y) {
                if (board.is_valid_move(x, y)) {
                    valid_moves.push_back(std::make_pair(x, y));
                }
            }
        }

        if (valid_moves.empty()) {
            break;
        }

        std::uniform_int_distribution<> dis(0, static_cast<int>(valid_moves.size() - 1));
        std::pair<int, int> random_move = valid_moves[dis(gen)];
        board.make_move(random_move.first, random_move.second, node->player);
    }
}

void MCTSAgent::backpropagate(const std::shared_ptr<Node>& node, char winner) {
    std::shared_ptr<Node> current_node = node;
    while (current_node) {
        current_node->visits++;
        if (current_node->player == winner) {
            current_node->wins++;
        }
        current_node = current_node->parent;
    }
}

