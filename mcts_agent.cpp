#include <iostream>
#include <limits>
#include <memory>
#include <cmath>
#include <random>
#include <cassert>
#include <iomanip>
#include <thread>
#include <mutex>
#include "mcts_agent.h"

Mcts_agent::Mcts_agent(double exploration_factor,
    std::chrono::milliseconds max_decision_time, bool is_parallelized, bool is_verbose) : exploration_factor(exploration_factor),
    max_decision_time(max_decision_time), is_verbose(is_verbose), random_generator(random_device()) {
    if (is_parallelized && is_verbose)
    {
        throw std::invalid_argument("Parallelization and verbose mode do not make sense together.");
    }
}

Mcts_agent::Node::Node(char player, std::pair<int, int> move,
    std::shared_ptr<Node> parent_node) :
    win_count(0),
    visit_count(0),
    move(move),
    player(player),
    child_nodes(),
    parent_node(parent_node) {}

std::pair<int, int> Mcts_agent::choose_move(const Board& board, char player)
{
    if (is_verbose)
    {
        std::cout << "\n-------------MCTS VERBOSE START - " << player << " to move-------------" << std::endl;
    }
    root = std::make_shared<Node>(player, std::make_pair(-1, -1), nullptr);
    std::vector<std::thread> threads;
    unsigned int number_of_threads;
    if (is_parallelized) {
        number_of_threads = std::thread::hardware_concurrency();
    }
    int mcts_iteration_counter = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto end_time = start_time + max_decision_time;
    expand_node(root, board);
    while (std::chrono::high_resolution_clock::now() < end_time)
    {
        std::shared_ptr<Node> chosen_child = select_child(root);
        if (is_parallelized) {
            std::vector<char> results(number_of_threads);
            for (unsigned int thread_index = 0; thread_index < number_of_threads; thread_index++) {
                threads.push_back(std::thread([&, thread_index]() {
                    results[thread_index] = simulate_random_playout(chosen_child, board);
                    }));
            }
            for (auto& thread : threads) {
                thread.join();
            }
            threads.clear();
            for (char playout_winner : results) {
                backpropagate(chosen_child, playout_winner);
            }
        }
        else {
            char playout_winner = simulate_random_playout(chosen_child, board);
            backpropagate(chosen_child, playout_winner);
        }
        if (is_verbose)
        {
            std::cout << "\nAFTER BACKPROP, root node has " << root->visit_count << " visit_count, " << root->win_count << " win_count, and " << root->child_nodes.size() << " child_nodes. Their details are:\n";
            for (const auto& child : root->child_nodes)
            {
                double win_ratio = static_cast<double> (child->win_count) / child->visit_count;
                std::cout << "Child node " << child->move.first << "," << child->move.second << ": Wins: " << child->win_count << ", Visits: " << child->visit_count << ". Win ratio: " << win_ratio << std::endl;
            }
        }
        mcts_iteration_counter++;
    }
    if (is_verbose)
    {
        std::cout << "\nTIMER RAN OUT. " << mcts_iteration_counter << " iterations completed. CHOOSING A MOVE NEXT:" << std::endl;
    }
    double max_win_ratio = -1.;
    std::shared_ptr<Node> best_child;
    if (root->child_nodes.empty())
    {
        throw std::runtime_error("Root does not have child_nodes.");
    }
    for (const auto& child : root->child_nodes)
    {
        double win_ratio = static_cast<double> (child->win_count) / child->visit_count;
        if (is_verbose)
        {
            std::cout << "Child move: " << child->move.first << "," << child->move.second <<
                "has Win ratio: " << win_ratio << std::endl;
        }

        if (win_ratio > max_win_ratio)
        {
            max_win_ratio = win_ratio;
            best_child = child;
        }
    }
    if (!best_child)
    {
        throw std::runtime_error("Choose move did not find the best child.");
    }
    else if (is_verbose)
    {
        std::cout << "\nAfter " << mcts_iteration_counter << " iterations, best node is " << best_child->move.first << ", " << best_child->move.second << " with win ratio " << std::setprecision(5) << max_win_ratio << std::endl;
        std::cout << "\n--------------------MCTS VERBOSE END--------------------\n" << std::endl;
    }
    return best_child->move;
}

void Mcts_agent::expand_node(const std::shared_ptr<Node>& node, const Board& board)
{
    // If there's already a winner, no need to expand the node
    if (board.check_winner() != '.')
    {
        throw std::logic_error("Can't expand node: game already has a winner.");
    }

    std::vector<std::pair<int, int>> valid_moves = board.get_valid_moves();
    for (const auto& move : valid_moves)
    {
        std::shared_ptr<Node> new_child =
            std::make_shared<Node>(node->player, move, node);
        node->child_nodes.push_back(new_child);
        if (is_verbose)
        {
            std::cout << "\nEXPANDED ROOT'S CHILD: " << move.first << "," << move.second << std::endl;
        }
    }
    if (node->child_nodes.empty())
    {
        throw std::runtime_error("No valid moves found");
    }
}

std::shared_ptr<Mcts_agent::Node > Mcts_agent::select_child(const std::shared_ptr<Node>& parent_node)
{
    if (parent_node->child_nodes.empty())
    {
        throw std::logic_error("Root has no children.");
    }

    // Initialize best_child as the first child and calculate its UCT score
    std::shared_ptr<Node> best_child = parent_node->child_nodes[0];
    double max_score = (best_child->visit_count == 0) ? std::numeric_limits<double>::max() :
        static_cast<double> (best_child->win_count) / best_child->visit_count +
        exploration_factor * std::sqrt(std::log(parent_node->visit_count) / best_child->visit_count);
    // Start loop from the second child
    for (auto it = std::next(parent_node->child_nodes.begin()); it != parent_node->child_nodes.end(); ++it)
    {
        const auto& child = *it;
        double uct_score = (child->visit_count == 0) ? std::numeric_limits<double>::max() :
            static_cast<double> (child->win_count) / child->visit_count +
            exploration_factor * std::sqrt(std::log(parent_node->visit_count) / child->visit_count);

        if (uct_score > max_score)
        {
            max_score = uct_score;
            best_child = child;
        }
    }

    if (is_verbose)
    {
        std::cout << "SELECTED CHILD: " << best_child->move.first << ", " << best_child->move.second << "; UCT=" << max_score << std::endl;
    }

    return best_child;
}

char Mcts_agent::simulate_random_playout(const std::shared_ptr<Node>& node, Board board)
{
    char current_player = node->player;
    board.make_move(node->move.first, node->move.second, current_player);
    if (is_verbose)
    {
        std::cout << "\nSIMULATING A RANDOM PLAYOUT from the node " << node->move.first << ", " << node->move.second << ". Simulation board is in state:" << board;
    }

    Board simulation_board(board);	// Create a new board copy for the simulation
    while (board.check_winner() == '.')
    {
        current_player = (current_player == 'B') ? 'R' : 'B';
        std::vector<std::pair<int, int>> valid_moves = board.get_valid_moves();
        if (valid_moves.empty())
        {
            throw std::runtime_error("No valid moves for the simulation found.");
        }

        std::uniform_int_distribution < > dis(0, static_cast<int> (valid_moves.size() - 1));
        std::pair<int, int> random_move = valid_moves[dis(random_generator)];
        if (is_verbose)
        {
            std::cout << "\nCurrent player in simulation is " << current_player << " in Board state:\n" << board;
            std::cout << current_player << " makes random move " << random_move.first << "," << random_move.second << std::endl;
        }

        board.make_move(random_move.first, random_move.second, current_player);
        if (board.check_winner() != '.')
        {
            if (is_verbose)
            {
                std::cout << "\nDETECTED WIN for player " << current_player << " in Board state:\n" << board;
            }

            break;	// If the game has ended, break the loop.
        }
    }

    return current_player;
}

//in the current implementation traverses the tree chosen child
//to its root (1 level), but is suitable for traversing the whole tree.
void Mcts_agent::backpropagate(std::shared_ptr<Node>& node, char winner)
{
    std::shared_ptr<Node> current_node = node;
    while (current_node != nullptr)
    {
        std::lock_guard<std::mutex> lock(current_node->node_mutex); // Lock the node's mutex
        current_node->visit_count += 1;
        if (winner == current_node->player)
        {
            current_node->win_count += 1;
        }

        if (is_verbose)
        {
            std::cout << "\nBACKPROPAGATED RESULT to node " << current_node->move.first << ", " << current_node->move.second << ". It currently has " << current_node->win_count << " win_count and " << current_node->visit_count << " visit_count." << std::endl;
        }

        current_node = current_node->parent_node;
    }
}

char Mcts_agent::get_opponent(char current_player) const
{
    return (current_player == 'B') ? 'R' : 'B';
}