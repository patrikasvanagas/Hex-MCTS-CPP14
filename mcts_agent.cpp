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
    Board simulation_board(board);
    //create a root node which contains the colour of the player, a filler move (-1, -1), and its parent
    //is a nullptr
    root = std::make_shared<Node>(player, std::make_pair(-1, -1), nullptr);
    //add all of the children nodes of the root node to its children vector
    expand_root(root, board);
    //start the timer for the while loop to run
    auto start_time = std::chrono::high_resolution_clock::now();
    auto end_time = start_time + move_time_limit;
    while (std::chrono::high_resolution_clock::now() < end_time) {
        Board simulated_board(board);
        //select the child with the highest ucb score.
        std::shared_ptr<Node> chosen_child = select_child(root);
        char playout_winner = simulate_random_playout(chosen_child, simulation_board);
        backpropagate(root, chosen_child, playout_winner);
        int foo = 5;

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


//finds the possible moves from the root and populates its
//children vector with the nodes of the allowed moves with
//the color of the current player.
void MCTSAgent::expand_root(
    const std::shared_ptr<Node>& node, const Board& board) {
    Board expanded_board(board);
    // If there's already a winner, no need to expand the node
    if (expanded_board.check_winner() != '.') {
        throw std::runtime_error("Can't expand root: game already has a winner.");
    }
    // Iterate through each possible move on the board
    for (int x = 0; x < expanded_board.get_board_size(); ++x) {
        for (int y = 0; y < expanded_board.get_board_size(); ++y) {
            // If the move is valid, create a new child node for it
            if (expanded_board.is_valid_move(x, y)) {
                std::shared_ptr<Node> new_child =
                    std::make_shared<Node>(node->player, std::make_pair(x, y), node);
                node->children.push_back(new_child);
                if (verbose) {
                    std::cout << "\EXPANDED ROOT'S CHILD: " << x << "," << y << std::endl;
                }
            }
        }
    }
    // If there are no valid moves, throw an exception
    if (node->children.empty()) {
        throw std::runtime_error("No valid moves found");
    }
}

//returns the child of the root node with the highest UCB score.
std::shared_ptr<MCTSAgent::Node> MCTSAgent::select_child(const std::shared_ptr<Node>& node)
{
    if (node->children.empty()) {
        return nullptr; // Or throw an exception
    }
    // Initialize best_child as the first child and calculate its UCT score
    std::shared_ptr<Node> best_child = node->children[0];
    double max_score = static_cast<double>(best_child->wins) / best_child->visits +
        exploration_constant * std::sqrt(std::log(node->visits) / best_child->visits);
    // Start loop from the second child
    for (auto it = std::next(node->children.begin()); it != node->children.end(); ++it) {
        const auto& child = *it;
        double uct_score = static_cast<double>(child->wins) / child->visits +
            exploration_constant * std::sqrt(std::log(node->visits) / child->visits);
        if (uct_score > max_score) {
            max_score = uct_score;
            best_child = child;
        }
    }
    if (verbose) {
        std::cout << "SELECTED CHILD: " << best_child->move.first << ", " << best_child->move.second << "; UCT=" << max_score << std::endl;
    }
    return best_child;
}


//takes in the chosen best child and the current state of the board. Does a random playout
//and returns the winner of the playout as a char
char MCTSAgent::simulate_random_playout(const std::shared_ptr<Node>& node, Board& board) {
    char current_player = node->player;
    board.make_move(node->move.first, node->move.second, current_player);
    if (verbose) {
        std::cout << "\nSIMULATING A RANDOM PLAYOUT from the node " << node->move.first << ", " << node->move.second << ". Simulation board is in state:" << board;
    }
    Board simulation_board(board);  // Create a new board copy for the simulation
    while (board.check_winner() == '.') {
        current_player = (current_player == 'B') ? 'R' : 'B';
        std::vector<std::pair<int, int>> valid_moves;
        for (int x = 0; x < board.get_board_size(); ++x) {
            for (int y = 0; y < board.get_board_size(); ++y) {
                if (board.is_valid_move(x, y)) {
                    valid_moves.push_back(std::make_pair(x, y));
                }
            }
        }
        if (valid_moves.empty()) {
            throw std::runtime_error("No valid moves for the simulation found.");
        }
        std::uniform_int_distribution<> dis(
            0, static_cast<int>(valid_moves.size() - 1));
        std::pair<int, int> random_move = valid_moves[dis(gen)];
        if (verbose) {
            std::cout << "\nCurrent player in simulation is " << current_player << " in Board state:\n" << board;
            std::cout << current_player << " makes random move " << random_move.first << "," << random_move.second << std::endl;
        }
        board.make_move(random_move.first, random_move.second, current_player);
        if (board.check_winner() != '.') {
            if (verbose) {
                std::cout << "\nDETECTED WIN for player " << current_player << " in Board state:\n" << board;
            }
            break;  // If the game has ended, break the loop.
        }
    }
    return current_player;
}

//adds one to visits of chosen child and parent and one to the wins of the child if it won.
void MCTSAgent::backpropagate(std::shared_ptr<Node>& parent, std::shared_ptr<Node>& child, char winner) {
    parent->visits += 1;
    child->visits += 1;
    if (winner == child->player) {
        child->wins += 1;
    }
    if (verbose) {
        std::cout << "\BACKPROPAGATED node" << child->move.first << ", " << child->move.second << ". It currently has " << child->wins << " wins and " << child->visits << " visits." << std::endl;
    }
}