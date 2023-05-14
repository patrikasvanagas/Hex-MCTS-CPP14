#include "console_interface.h"

bool is_integer(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

char get_yes_or_no_response(const std::string& prompt) {
    char response;
    while (true) {
        std::cout << prompt;
        std::cin >> response;
        response = std::tolower(response); // convert to lower case to allow both 'Y' and 'y', etc.

        if (std::cin.fail()) {
            std::cin.clear(); // clear the error state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ignore the rest of the line
            std::cout << "Invalid input. Please enter 'y' or 'n'.\n";
        }
        else if (response != 'y' && response != 'n') {
            std::cout << "Invalid response. Please enter 'y' or 'n'.\n";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear the rest of the line
            return response;
        }
    }
}

template <>
int get_parameter_within_bounds<int>(const std::string& prompt, int lower_bound, int upper_bound) {
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
        }
        else {
            break;
        }
    }

    return value;
}

template <>
double get_parameter_within_bounds<double>(const std::string& prompt, double lower_bound, double upper_bound) {
    std::string input;
    double value;

    while (true) {
        std::cout << prompt;
        std::cin >> input;

        // Check if input is a valid double
        try {
            value = std::stod(input);
        }
        catch (std::invalid_argument&) {
            std::cout << "Invalid input. Please enter a valid number.\n";
            continue;
        }

        // Check if value is within bounds
        if (!is_in_bounds(value, lower_bound, upper_bound)) {
            std::cout << "Invalid value. Please try again.\n";
        }
        else {
            break;
        }
    }

    return value;
}

void play_against_robot() {
    int human_number = get_parameter_within_bounds("Enter '1' if you want to be Player 1 (Blue) or '2' if you want to be Player 2 (Red): ", 1, 2);
    int board_size = get_parameter_within_bounds("Enter board size (between 2 and 11): ", 2, 11);
    int max_decision_time_ms = get_parameter_within_bounds("Enter max decision time for the robot in milliseconds (at least 100): ", 100, INT_MAX);

    double exploration_constant = 1.41;
    if (get_yes_or_no_response("Would you like to change the default exploration constant (1.41)? (y/n): ") == 'y') {
        exploration_constant = get_parameter_within_bounds("Enter exploration constant (between 0.1 and 2): ", 0.1, 2.0);
    }
    bool is_parallelized = (get_yes_or_no_response("Would you like to parallelize the agent? (y/n): ") == 'y');
    bool is_verbose = false;
    if (!is_parallelized) {
        bool is_verbose = (get_yes_or_no_response("Would you like to enable verbose mode? (y/n): ") == 'y');
	}
    auto human_player = std::make_unique<Human_player>();
    auto mcts_agent_player = std::make_unique<Mcts_player>(exploration_constant, std::chrono::milliseconds(max_decision_time_ms), is_parallelized, is_verbose);

    if (human_number == 1) {
        Game game(board_size, std::move(human_player), std::move(mcts_agent_player));
        game.play();
    }
    else {
        Game game(board_size, std::move(mcts_agent_player), std::move(human_player));
        game.play();
    }
}


void start_robot_arena() {
    int board_size = get_parameter_within_bounds("Enter board size (between 2 and 11): ", 2, 11);

    std::cout << "Enter parameters for MCTS Agent 1:\n";
    double exploration_constant1;
    std::cout << "Enter exploration constant (between 0.5 and 2, recommended 1.4): ";
    std::cin >> exploration_constant1;

    int decision_time1;
    std::cout << "Enter maximum decision time in milliseconds (at least 100): ";
    std::cin >> decision_time1;

    bool is_parallelized1;
    std::cout << "Enter 1 for parallelized mode, 0 for not parallelized: ";
    std::cin >> is_parallelized1;

    bool is_verbose1;
    std::cout << "Enter 1 for verbose mode, 0 for not verbose: ";
    std::cin >> is_verbose1;

    std::cout << "Enter parameters for MCTS Agent 2:\n";
    double exploration_constant2;
    std::cout << "Enter exploration constant (between 0.5 and 2, recommended 1.4): ";
    std::cin >> exploration_constant2;

    int decision_time2;
    std::cout << "Enter maximum decision time in milliseconds (at least 100): ";
    std::cin >> decision_time2;

    bool is_parallelized2;
    std::cout << "Enter 1 for parallelized mode, 0 for not parallelized: ";
    std::cin >> is_parallelized2;

    bool is_verbose2;
    std::cout << "Enter 1 for verbose mode, 0 for not verbose: ";
    std::cin >> is_verbose2;

    auto mcts_agent_player1 = std::make_unique<Mcts_player>(exploration_constant1, std::chrono::milliseconds(decision_time1), is_parallelized1, is_verbose1);
    auto mcts_agent_player2 = std::make_unique<Mcts_player>(exploration_constant2, std::chrono::milliseconds(decision_time2), is_parallelized2, is_verbose2);

    Game game(board_size, std::move(mcts_agent_player1), std::move(mcts_agent_player2));
    game.play();
}


void start_human_arena() {
    // TODO: Get input parameters and start human arena game
}

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

void print_docs() {
    std::cout << "\Hex is a two-person, zero-sum perfect information game without chance. It has a rather cult following in the community.\n\n";
}

void print_exit_ascii_art() {
    std::cout << R"(
  ___                      _  _                                  _   ___ 
 | _ )  _  _   ___        | || |  _  _   _ __    __ _   _ _     (_) | _ \
 | _ \ | || | / -_)  _    | __ | | || | | '  \  / _` | | ' \     _  |  _/
 |___/  \_, | \___| ( )   |_||_|  \_,_| |_|_|_| \__,_| |_||_|   (_) |_|  
        |__/        |/                                                                                                              
     
)" << '\n';
}

void run_console_interface() {
    
        print_welcome_ascii_art();
        std::cout << "Welcome, Human.\n\n";

        bool is_running = true;
        while (is_running) {
        try {
            int option = 0;
            std::cout << "SELECT AN OPTION:\n"
                << "1) Play against a robot\n"
                << "2) Robot arena\n"
                << "3) Human arena\n"
                << "4) Read the docs\n"
                << "5) Exit\n";

            option = get_parameter_within_bounds("Option: ", 1, 5);
            std::cout << "\n";

            switch (option) {
            case 1:
                play_against_robot();
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
        }
        catch (const std::invalid_argument& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
        catch (const std::logic_error& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
        catch (const std::runtime_error& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
    print_exit_ascii_art();
}

