#include <limits>
#include <memory>
#include <cmath>
#include <random>
#include <iostream>
#include <cassert>
#include <iomanip>
#include "mcts_agent.h"

MCTSAgent::MCTSAgent(double exploration_constant,
    std::chrono::milliseconds move_time_limit, bool verbose)
    : exploration_constant(exploration_constant),
    move_time_limit(move_time_limit), verbose(verbose), gen(rd()) {}

MCTSAgent::Node::Node(char player, std::pair<int, int> move,
    std::shared_ptr<Node> parent)
    : wins(0),
    visits(0),
    move(move),
    player(player),
    children(),
    parent(parent) {}

char MCTSAgent::get_opponent(char player) const {
    return (player == 'B') ? 'R' : 'B';
}

std::pair<int, int> MCTSAgent::choose_move(const Board& board, char player) {
    if (board.check_winner() != '.') {
        throw std::runtime_error("Game has already ended.");
    }
    int MCTS_iteration_counter = 0;
    if (verbose) {
        
		std::cout << "\n-------------MCTS VERBOSE START - " << player << " to move-------------" << std::endl;
	}
    root = std::make_shared<Node>(player, std::make_pair(-1, -1), nullptr);
    auto start_time = std::chrono::high_resolution_clock::now();
    auto end_time = start_time + move_time_limit;
    while (std::chrono::high_resolution_clock::now() < end_time) {
        std::shared_ptr<Node> node = root;
        Board simulated_board(board);
        while (node->children.size() > 0 && node->visits > 0) {
            node = select_node(node, simulated_board);
        }
        if (simulated_board.check_winner() == '.') {
            node = expand_node(node, simulated_board);
            if (node) {
                char opponent = (node->player == 'B') ? 'R' : 'B';
                simulate_random_playout(simulated_board, opponent);
            }
        }
        char winner = simulated_board.check_winner();
        backpropagate(node, winner);
        if (verbose) {
            std::cout << "\nAfter backprop, root node has " << root->children.size() << " children. Their details are:\n";
            for (const auto& child : root->children) {
                double win_ratio = static_cast<double>(child->wins) / child->visits;
                std::cout << "Child move: " << child->move.first << "," << child->move.second << " - Wins: " << child->wins << " Visits: " << child->visits << " Win ratio: " << win_ratio << std::endl;
            }
        }

        MCTS_iteration_counter++;

    }
    double max_win_ratio = -1.;
    std::shared_ptr<Node> best_child;
    if (root->children.empty()) {
        throw std::runtime_error("No legal moves available.");
    }
    for (const auto& child : root->children) {
        double win_ratio = static_cast<double>(child->wins) / child->visits;
        if (verbose) {
            std::cout << "choose_move verbose: Child move: " << child->move.first << "," << child->move.second
                << " Win ratio: " << win_ratio << std::endl;
        }
        if (win_ratio > max_win_ratio) {
            max_win_ratio = win_ratio;
            best_child = child;
        }
    }
    if (!best_child) {
        throw std::runtime_error("No legal moves available.");
    }
    else if (verbose) {
        std::cout << "\nAfter " << MCTS_iteration_counter << " iterations, best node is " << best_child->move.first << ", " << best_child->move.second << " with win ratio " << std::setprecision(5) << max_win_ratio << std::endl;
		std::cout << "\n--------------------MCTS VERBOSE END--------------------\n" << std::endl;
	}
    return best_child->move;
}


std::shared_ptr<MCTSAgent::Node> MCTSAgent::select_node(
    const std::shared_ptr<Node>& node, const Board& board) {
    double max_score = std::numeric_limits<double>::lowest();
    std::shared_ptr<Node> best_child;
    for (const auto& child : node->children) {
        double uct_score = static_cast<double>(child->wins) / child->visits +
            exploration_constant *
            std::sqrt(std::log(node->visits) / child->visits);
        if (verbose) {
            std::cout << "select_node verbose: Child move: " << child->move.first << "," << child->move.second
                << " UCT score: " << uct_score << std::endl;
        }
        if (uct_score > max_score) {
            max_score = uct_score;
            best_child = child;
        }
    }
    return best_child;
}

std::shared_ptr<MCTSAgent::Node> MCTSAgent::expand_node(
    const std::shared_ptr<Node>& node, Board& board) {
    Board expanded_board(board);
    if (node->move.first != -1 && node->move.second != -1) {
        expanded_board.make_move(node->move.first, node->move.second, node->player);
    }
    if (expanded_board.check_winner() != '.') {
        return nullptr;
    }
    char next_player = (node->player == 'B') ? 'R' : 'B';
    for (int x = 0; x < expanded_board.get_board_size(); ++x) {
        for (int y = 0; y < expanded_board.get_board_size(); ++y) {
            if (expanded_board.is_valid_move(x, y)) {
                std::shared_ptr<Node> new_child =
                    std::make_shared<Node>(next_player, std::make_pair(x, y), node);
                node->children.push_back(new_child);
                if (verbose) {
                    std::cout << "expand_node verbose: Expanded child: " << x << "," << y << std::endl;
                }
            }
        }
    }
    if (node->children.empty()) {
        return nullptr;
    }
    std::uniform_int_distribution<> dis(
        0, static_cast<int>(node->children.size() - 1));
    return node->children[dis(gen)];
}


void MCTSAgent::simulate_random_playout(Board& board, char current_player) {
    bool has_winner = false;
    //Board simulation_board(board);  // Create a new board copy for the simulation
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
        std::uniform_int_distribution<> dis(
            0, static_cast<int>(valid_moves.size() - 1));
        std::pair<int, int> random_move = valid_moves[dis(gen)];
        if (verbose && !has_winner) {
            std::cout << "\nSIMULATING A RANDOM PLAYOUT: Current player is " << current_player << " in Board state:\n" << board;
            std::cout << "Choosing random move " << random_move.first << "," << random_move.second << std::endl;
        }
        board.make_move(random_move.first, random_move.second, current_player);
        if (board.check_winner() != '.') {
            has_winner = true;
        }
        if (has_winner) {
            if (verbose) {
                std::cout << "\nverbose simulate_random_playout: Board state:\n" << board << std::endl;
                std::cout << "DETECTED WINNER in random playout: " << board.check_winner() << "\n\n";
            }
            break;  // If the game has ended, break the loop.
        }
        else {
            current_player = (current_player == 'B') ? 'R' : 'B';
        }
    }
}


void MCTSAgent::backpropagate(const std::shared_ptr<Node>& node, char winner) {
    std::shared_ptr<Node> current_node = node;
    while (current_node && current_node->parent) { // Skip root
        current_node->visits++;
        if (get_opponent(current_node->player) == winner) {
            current_node->wins++;
        }
        if (verbose) {
            std::cout << "BACKPROPAGATING MOVE: " << current_node->move.first << "," << current_node->move.second
                << " wins: " << current_node->wins << " visits: " << current_node->visits << std::endl;
        }
        current_node = current_node->parent;
    }
}


