#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <cstdlib>
#include <algorithm>
#include <chrono>

using namespace std;

void display_board(const vector<int>& gameboard) {
    for (int i = 0; i < 9; ++i) {
        // Draw "O" for player one, "X" for player two, and " " for blank spaces
        if (gameboard[i] == 1) {
            cout << "O";
        } else if (gameboard[i] == -1) {
            cout << "X";
        } else {
            cout << " ";
        }
        if ((i + 1) % 3 == 0) {
            cout << endl;
            if (i < 8) {
                cout << "---------" << endl; // Horizontal line to separate rows
            }
        } else {
            cout << " | ";
        }
    }
}

bool check_win(const vector<int>& gameboard, const int& player) {
    vector<vector<int>> win_conditions = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};

    for (const auto& condition : win_conditions) {
        if (gameboard[condition[0]] == player && gameboard[condition[1]] == player && gameboard[condition[2]] == player) {
            return true;
        }
    }

    return false;
}

vector<int> get_possible_moves(const vector<int>& gameboard) {
    vector<int> possible_moves;
    for (int i = 0; i < 9; ++i) {
        if (gameboard[i] == 0) {
            possible_moves.push_back(i);
        }
    }
    return possible_moves;
}

int negamax(vector<int> gameboard, int player, int depth, int alpha, int beta) {
    // Terminal node checks
    if (check_win(gameboard, player)) {
        return depth; // Winning sooner is better
    }
    
    if (check_win(gameboard, -player)) {
        return depth-9; // Try to lose as late in the game as possible
    }
    
    if (std::find(gameboard.begin(), gameboard.end(), 0) == gameboard.end()) {
        return 0; // Draw game
    }
    
    if (depth == 0) {
        return 0; // We've searched as far as the depth will let us. Just return 0 because we don't actually know how good the node is.
    }
    
    // If not a terminal node, continue recursively searching the game tree
    int score = -100;
    int best_score = -100;
    
    for (int move : get_possible_moves(gameboard)) {
        gameboard[move] = player;
        score = -negamax(gameboard, -player, depth-1, -beta, -alpha);
        gameboard[move] = 0;
        
        if (score > best_score) {
            best_score = score;
        }
        
        alpha = max(alpha, score);
        
        // Alpha-beta pruning
        if (alpha >= beta) {
            break;
        }
    }
    
    return best_score;
}

int solve(vector<int> gameboard, int player, int depth) {
    int best_move;
    int score = -100;
    int best_score = -100;
    int alpha = -100; // Initial alpha value
    int beta = 100;   // Initial beta value
    
    for (int move : get_possible_moves(gameboard)) {
        gameboard[move] = player;
        score = -negamax(gameboard, -player, depth-1, -beta, -alpha);
        gameboard[move] = 0;
        
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
        
        alpha = max(alpha, score);
    }
    
    return best_move;
}

// Human player is 1
// AI player is -1
int main() {
    vector<int> gameboard(9, 0);
    string input;
    int move, turn;
    
    cout << "Would you like to be player 1 or 2 (enter 'exit' to quit): ";
    while (true) {
        cin >> input; // Get player input
        transform(input.begin(), input.end(), input.begin(), ::tolower); // Make it lowercase for checks
        if (input == "1") {
            turn = 1;
            break;
        } else if (input == "2") {
            turn = -1;
            break;
        } else if (input == "exit") {
            exit(0);
        } else {
            cout << "Invalid input. Please enter '1' or '2' or 'exit': ";
        }
    }
    
    while (true) {
        display_board(gameboard);
        cout << endl;
        
        if (turn == 1) {

            // Get player input
            while (true) {
                cout << "Enter move (1-9, or 'exit' to quit): ";
                cin >> input;
                transform(input.begin(), input.end(), input.begin(), ::tolower); // Make it lowercase for checks

                if (input == "exit") {
                    exit(0);
                }

                stringstream ss(input);
                if (!(ss >> move) || move < 1 || move > 9 || gameboard[move - 1] != 0) {
                    cout << "Invalid input. Please enter a number between 1 and 9 or 'exit'." << endl;
                    continue;
                }

                gameboard[move - 1] = 1;
                break;
            }

            // Check if the human player has won
            if (check_win(gameboard, 1)) {
                display_board(gameboard);
                cout << endl << "Human player wins!" << endl;
                exit(0);
            }

            if (std::find(gameboard.begin(), gameboard.end(), 0) == gameboard.end()) {
                display_board(gameboard);
                cout << endl << "Game was a draw." << endl;
                exit(0);
            }
            
            turn = -1;
        
        } else { // Turn = -1
            
            // Get the AI move
            auto start_time = std::chrono::high_resolution_clock::now(); // Start measuring time
            move = solve(gameboard, -1, 9);
            gameboard[move] = -1;
            auto end_time = std::chrono::high_resolution_clock::now();  // Stop measuring time
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            cout << "AI move took " << duration.count() << " milliseconds to calculate." << endl << endl; // Show time to calculate move

            if (check_win(gameboard, -1)) {
                display_board(gameboard);
                cout << "AI player wins!" << endl;
                exit(0);
            }

            if (std::find(gameboard.begin(), gameboard.end(), 0) == gameboard.end()) {
                display_board(gameboard);
                cout << endl << "Game was a draw." << endl;
                exit(0);
            }
            
            turn = 1;
        
        }
    }
    
    return 0;
}
