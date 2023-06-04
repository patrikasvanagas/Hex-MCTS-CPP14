#include "console_interface.h"

#include <chrono>
#include <thread>

#include "board.h"

/**
 * @brief Checks if the input string is an integer.
 *
 * This function checks if all characters in a string are digits,
 * thus determining if the string represents a whole number.
 *
 * @param s The string to check.
 * @return true if the string is an integer, false otherwise.
 */
bool is_integer(const std::string& s) {
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it)) ++it;
  return !s.empty() && it == s.end();
}

/**
 * @brief Prompt the user for a 'yes' or 'no' response.
 *
 * This function takes a prompt string, displays it, and reads the user's
 * response. It ensures the user's input is valid ('y', 'n', 'Y', or 'N') before
 * returning.
 *
 * @param prompt The string to display to the user.
 * @return Lowercase 'y' or 'n' based on user's response.
 */
char get_yes_or_no_response(const std::string& prompt) {
  char response;
  while (true) {
    std::cout << prompt;
    std::cin >> response;

    if (std::cin.fail()) {
      std::cin.clear();  // clear the error state
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                      '\n');  // ignore the rest of the line
      std::cout << "Invalid input. Please enter 'y' or 'n'.\n";
    } else if (response != 'y' && response != 'n' && response != 'Y' &&
               response != 'N') {
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                      '\n');  // clear the rest of the line
      std::cout << "Invalid response. Please enter 'y' or 'n'.\n";
    } else {
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      return std::tolower(response);
    }
  }
}

/**
 * @brief Template specializations of get_parameter_within_bounds for int and
 * double types.
 *
 * These functions prompt the user for an input within specified bounds.
 * The user is repeatedly asked until a valid response is given.
 *
 * @param prompt The string to display to the user.
 * @param lower_bound The inclusive lower bound.
 * @param upper_bound The inclusive upper bound.
 * @return The user's input as an int or a double.
 */
template <>
int get_parameter_within_bounds<int>(const std::string& prompt, int lower_bound,
                                     int upper_bound) {
  std::string input;
  int value;

  while (true) {
    std::cout << prompt;
    std::cin >> input;

    // Check if input is a valid integer
    if (!is_integer(input)) {
      std::cout << "Invalid input. Please enter a valid integer.\n";
      continue;
    }

    // Convert string to int
    value = std::stoi(input);

    // Check if value is within bounds
    if (!is_in_bounds(value, lower_bound, upper_bound)) {
      std::cout << "Invalid value. Please try again.\n";
    } else {
      break;
    }
  }

  return value;
}

template <>
double get_parameter_within_bounds<double>(const std::string& prompt,
                                           double lower_bound,
                                           double upper_bound) {
  std::string input;
  double value;

  while (true) {
    std::cout << prompt;
    std::cin >> input;

    // Check if input is a valid double
    try {
      value = std::stod(input);
    } catch (std::invalid_argument&) {
      std::cout << "Invalid input. Please enter a valid number.\n";
      continue;
    }

    // Check if value is within bounds
    if (!is_in_bounds(value, lower_bound, upper_bound)) {
      std::cout << "Invalid value. Please try again.\n";
    } else {
      break;
    }
  }

  return value;
}

/**
 * @brief Creates a Monte Carlo Tree Search (MCTS) player with custom
 * parameters.
 *
 * This function prompts the user for various parameters to initialize the MCTS
 * agent, such as maximum decision time, exploration constant, parallelization,
 * and verbosity.
 *
 * @param agent_prompt The string used to indicate the agent being initialized.
 * @return A unique pointer to the MCTS agent.
 */
std::unique_ptr<Mcts_player> create_mcts_agent(
    const std::string& agent_prompt) {
  std::cout << "\nInitializing " << agent_prompt << ":\n";

  int max_decision_time_ms = get_parameter_within_bounds(
      "Enter max decision time in milliseconds (at least 100): ", 100, INT_MAX);

  double exploration_constant = 1.41;
  if (get_yes_or_no_response("Would you like to change the default exploration "
                             "constant (1.41)? (y/n): ") == 'y') {
    exploration_constant = get_parameter_within_bounds(
        "Enter exploration constant (between 0.1 and 2): ", 0.1, 2.0);
  }

  bool is_parallelized =
      (get_yes_or_no_response(
           "Would you like to parallelize the agent? (y/n): ") == 'y');

  bool is_verbose = false;
  if (!is_parallelized) {
    is_verbose = (get_yes_or_no_response(
                      "Would you like to enable verbose mode? (y/n): ") == 'y');
  }

  return std::make_unique<Mcts_player>(
      exploration_constant, std::chrono::milliseconds(max_decision_time_ms),
      is_parallelized, is_verbose);
}

/**
 * @brief A simple countdown function.
 *
 * This function performs a countdown from the specified number of seconds to 0.
 * It is used before the start of Robot vs Human game when verbose mode is
 * enabled.
 *
 * @param seconds The number of seconds to count down from.
 */
void countdown(int seconds) {
  while (seconds > 0) {
    std::cout << "The agent will start thinking loudly in " << seconds
              << " ...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    --seconds;
  }
}

/**
 * @brief Start a game against an MCTS agent.
 *
 * This function prompts the user for the desired player number and board size.
 * It then creates an MCTS agent and a human player, and starts a game.
 */
void start_match_against_robot() {
  int human_player_number = get_parameter_within_bounds(
      "Enter '1' if you want to be Player 1 (Blue, Vertical) or '2' if you "
      "want to be "
      "Player 2 (Red, Horizontal): ",
      1, 2);
  int board_size = get_parameter_within_bounds(
      "Enter board size (between 2 and 11): ", 2, 11);

  auto mcts_agent = create_mcts_agent("agent");
  auto human_player = std::make_unique<Human_player>();

  if (human_player_number == 1) {
    Game game(board_size, std::move(human_player), std::move(mcts_agent));
    game.play();
  } else {
    if (mcts_agent->get_is_verbose()) {
      countdown(3);
    }
    Game game(board_size, std::move(mcts_agent), std::move(human_player));
    game.play();
  }
}

/**
 * @brief Start a game between two MCTS agents.
 *
 * This function prompts the user for the board size, creates two MCTS agents,
 * and starts a game between them.
 */
void start_robot_arena() {
  int board_size = get_parameter_within_bounds(
      "Enter board size (between 2 and 11): ", 2, 11);

  auto mcts_agent_1 = create_mcts_agent("first agent");
  auto mcts_agent_2 = create_mcts_agent("second agent");

  Game game(board_size, std::move(mcts_agent_1), std::move(mcts_agent_2));
  game.play();
}

/**
 * @brief Start a game between two human players.
 *
 * This function prompts the user for the board size, creates two human players,
 * and starts a game between them.
 */
void start_human_arena() {
  int board_size = get_parameter_within_bounds(
      "Enter board size (between 2 and 11): ", 2, 11);
  auto human_player_1 = std::make_unique<Human_player>();
  auto human_player_2 = std::make_unique<Human_player>();
  Game game(board_size, std::move(human_player_1), std::move(human_player_2));
  game.play();
}

/**
 * @brief Run the console interface of the game.
 *
 * This function drives the main loop of the console interface.
 * It displays a menu and handles user's selection,
 * allowing the user to start different types of games or read the game docs.
 * It also handles exceptions and displays appropriate error messages.
 */
void run_console_interface() {
  print_welcome_ascii_art();
  std::cout << "Welcome.\n";

  bool is_running = true;
  while (is_running) {
    try {
      int option = 0;
      std::cout << "\nMENU:\n"
                << "\n[1] Play against a robot\n"
                << "[2] Robot arena\n"
                << "[3] Human arena\n"
                << "[4] Read the docs\n"
                << "[5] (H)Exit\n";

      option = get_parameter_within_bounds("Option: ", 1, 5);
      std::cout << "\n";

      switch (option) {
        case 1:
          start_match_against_robot();
          break;
        case 2:
          start_robot_arena();
          break;
        case 3:
          start_human_arena();
          break;
        case 4:
          print_docs();
          break;
        case 5:
          is_running = false;
          break;
        default:
          break;
      }
    } catch (const std::invalid_argument& e) {
      std::cout << "Error: " << e.what() << "\n";
    } catch (const std::logic_error& e) {
      std::cout << "Error: " << e.what() << "\n";
    } catch (const std::runtime_error& e) {
      std::cout << "Error: " << e.what() << "\n";
    }
  }
  print_exit_ascii_art();
}

/**
 * @brief Prints welcome message in ASCII art.
 */
void print_welcome_ascii_art() {
  std::cout << R"(

    )            )           *                       (     
 ( /(         ( /(         (  `       (      *   )   )\ )  
 )\())  (     )\())        )\))(      )\   ` )  /(  (()/(  
((_)\   )\   ((_)\     __ ((_)()\   (((_)   ( )(_))  /(_)) 
 _((_) ((_)  __((_)   / / (_()((_)  )\___  (_(_())  (_))   
| || | | __| \ \/ /  / /  |  \/  | ((/ __| |_   _|  / __|  
| __ | | _|   >  <  /_/   | |\/| |  | (__    | |    \__ \  
|_||_| |___| /_/\_\       |_|  |_|   \___|   |_|    |___/  
                                                       
)" << '\n';
}

/**
 * @brief Displays the Board object and the winner
 * from the pieces on the board.
 *
 * This function is used for demonstration purposes to show a winning condition.
 *
 * @param board The game board to display.
 */
void print_board_and_winner(Board& board) {
  board.display_board(std::cout);
  Cell_state winner = board.check_winner();
  std::cout << "Winner: " << winner << std::endl;
  std::cout << "------------------" << std::endl;
}

/**
 * @brief Allows to build Hex board configurations
 * to show the winning condition and calls
 * print_board_and_winner to display them.
 *
 * This function is used for demonstration purposes to show a winning condition.
 *
 */
void display_winning_condition() {
  // Demo case 1: 3x3 board, player Cell_state::Blue wins
  Board board_1(3);
  board_1.make_move(0, 2, Cell_state::Blue);
  board_1.make_move(1, 1, Cell_state::Blue);
  board_1.make_move(2, 1, Cell_state::Blue);
  print_board_and_winner(board_1);

  // Demo case 2: 3x3 board, player Cell_state::Red wins
  Board board_2(3);
  board_2.make_move(1, 0, Cell_state::Red);
  board_2.make_move(1, 1, Cell_state::Red);
  board_2.make_move(0, 2, Cell_state::Red);
  print_board_and_winner(board_2);

  // Demo case 2: 3x3 board, player Cell_state::Red wins
  Board board3(5);
  board3.make_move(3, 0, Cell_state::Red);
  board3.make_move(3, 1, Cell_state::Red);
  board3.make_move(2, 2, Cell_state::Red);
  board3.make_move(1, 3, Cell_state::Red);
  board3.make_move(1, 4, Cell_state::Red);
  print_board_and_winner(board3);
}

/**
 * @brief Displays some information about the
 * programme.
 *
 * Prints out a brief explanation of the Hex game,
 * how it is displayed in the console, and MCTS
 * agents.
 */
void print_docs() {
  std::cout << R"(
Hex is a two-player, zero-sum, perfect information game invented by the Danish mathematician Piet Hein and independently by the American mathematician John Nash. As it is a deterministic strategy game, chance plays no part in Hex, and unlike in chess or checkers, there are no 'draw' outcomes in Hex - there is always a winner and a loser.

The game is played on a rhombus-shaped board divided into hexagonal cells. The standard game board sizes are 11x11 or 13x13, but the size can be any square board from 2x2 up to 19x19 for tournament rules.

Each player is assigned a pair of opposite sides of the board, and the goal of each player is to form a connected path of their own stones linking their two sides. Usually, the blue player goes first and tries to create a vertical path, while the red player goes second and tries to create horizontal path. The player who completes their path first is the winner. The game does not allow for ties, and, given perfect play by both players, the first player can always win.

The game requires strategic depth as players must balance between extending their own path and blocking their opponent. Although the rules are straightforward, the strategic complexity becomes apparent as you gain experience.

In this console implementation, the connections between the cells are displayed by hyphens and slashes. Let's look at how the board is displayed and some sample winning conditions:
)" << std::endl;

  display_winning_condition();

  std::cout << R"(
The robots in this implementation are powered by a AI agent using a powerful strategy known as Monte Carlo Tree Search (MCTS). The MCTS is a heuristic search algorithm known for its effectiveness in decision-making problems, particularly in games like Hex.

This implementation of MCTS consists of four main phases:

1. Expansion: From the root node (representing the current game state), child nodes are found by detecting the moves allowed by the game state.

2. Selection: A child with the most promising score of Upper Confidence Bound applied to Trees (UCT) is selected for a random playout.

3. Simulation: A simulation is run from the child according to the default policy; in this case, a random game is played out.

4. Backpropagation: The result of the simulation is backpropagated through the tree. The parent and the chosen child node have their visit count incremented and their value updated.

This process is repeated until the computational budget (based on time) is exhausted. The agent then selects the move that leads to the most promising child node.

In this implementation, the MCTS agent also supports parallel simulations by running multiple threads, each executing an MCTS iteration. The non-parallelised agent can run in verbose mode, outputting detailed information about each MCTS iteration, which can be a valuable tool for understanding the decision-making process of the AI.

It should be noted that while MCTS does incorporate randomness (through the simulation phase), it is not a purely random algorithm. It uses the results of previous iterations to make informed decisions, and over time it builds a more accurate representation of the search space.

Remember - defense is offense. Good luck!

Author: Patrikas Vanagas, 2023
)" << std::endl;
}

/**
 * @brief Prints exit message in ASCII art.
 */
void print_exit_ascii_art() {
  std::cout << R"(

    __  ___               __  __            ___    ____   __                     _ __  __                        
   /  |/  /___ ___  __   / /_/ /_  ___     /   |  /  _/  / /_  ___     _      __(_) /_/ /_     __  ______  __  __
  / /|_/ / __ `/ / / /  / __/ __ \/ _ \   / /| |  / /   / __ \/ _ \   | | /| / / / __/ __ \   / / / / __ \/ / / /
 / /  / / /_/ / /_/ /  / /_/ / / /  __/  / ___ |_/ /   / /_/ /  __/   | |/ |/ / / /_/ / / /  / /_/ / /_/ / /_/ / 
/_/  /_/\__,_/\__, /   \__/_/ /_/\___/  /_/  |_/___/  /_.___/\___/    |__/|__/_/\__/_/ /_/   \__, /\____/\__,_/  
             /____/                                                                         /____/               

)" << '\n';
}
