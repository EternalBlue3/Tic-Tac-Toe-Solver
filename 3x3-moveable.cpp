#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <tuple>
#include <list>

using namespace std;

const vector<vector<int>> win_conditions = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};

void display_board(const vector<int>& gameboard, const list<int>& player_positions, const list<int>& ai_positions) {
    for (int i = 0; i < 9; ++i) {
        char symbol;
        
        if (gameboard[i] == 1) {
            symbol = 'O';
        } else if (gameboard[i] == -1) {
            symbol = 'X';
        } else {
            symbol = ' ';
        }

        if (!player_positions.empty() && i == player_positions.front()) {
            cout << "\033[31m" << symbol << "\033[0m";
        } else if (!ai_positions.empty() && i == ai_positions.front()) {
            cout << "\033[34m" << symbol << "\033[0m";
        } else {
            cout << symbol;
        }

        if ((i + 1) % 3 == 0) {
            cout << endl;
            if (i < 8) {
                cout << "---------" << endl;
            }
        } else {
            cout << " | ";
        }
    }
}

bool check_win(const vector<int>& gameboard, const int& player) {
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

int negamax(vector<int> gameboard, list<int> player_positions, list<int> opponent_positions, int player, int depth, int alpha, int beta) {
    
    // Terminal node checks
    if (check_win(gameboard, player)) {
        return depth;
    }
    
    if (check_win(gameboard, -player)) {
        return -depth;
    }
    
    if (depth == 0) {
        return 0;
    }
    
    int best_score = -10000; // Initial best score
    int score;
    
    // Copy values
    list<int> player_positions_orig = player_positions;
    vector<int> gameboard_orig = gameboard;
    
    for (int move : get_possible_moves(gameboard)) {
        // Make move and update positions variable
        gameboard[move] = player;
        player_positions.push_back(move);
        if (player_positions.size() == 4) {
            gameboard[player_positions.front()] = 0;
            player_positions.pop_front();
        }
        
        // Use a null window search by calling negamax with a narrow window
        score = -negamax(gameboard, opponent_positions, player_positions, -player, depth-1, -alpha-1, -alpha);
        
        // If the score is inside the new window, re-evaluate with a proper window
        if (alpha < score && score < beta) {
            score = -negamax(gameboard, opponent_positions, player_positions, -player, depth-1, -beta, -score);
        }
        
        // Reset move and player position
        gameboard = gameboard_orig;
        player_positions = player_positions_orig;
                
        if (score > best_score) {
            best_score = score;
        }
        
        alpha = max(alpha, score);
                
        if (alpha >= beta) {
            break;
        }
    }
    
    return best_score;
}

tuple<int, int> solve(vector<int> gameboard, list<int> player_positions, list<int> opponent_positions, int player, int depth) {
    int best_move, score;
    int best_score = -10000; // Initial best score
    int alpha = -10000; // Initial alpha range (lower bound)
    int beta = 10000;   // Initial beta range (upper bound)
    
    // Copy values
    list<int> player_positions_orig = player_positions;
    vector<int> gameboard_orig = gameboard;
        
    for (int move : get_possible_moves(gameboard)) {
        // Make move and update positions variable
        gameboard[move] = player;
        player_positions.push_back(move);
        if (player_positions.size() == 4) {
            gameboard[player_positions.front()] = 0;
            player_positions.pop_front();
        }
        
        // Use a null window search by calling negamax with a narrow window
        score = -negamax(gameboard, opponent_positions, player_positions, -player, depth-1, -beta, -alpha);
        
        // Reset move and player position
        gameboard = gameboard_orig;
        player_positions = player_positions_orig;
                
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
        
        // Update the alpha range based on the score
        alpha = max(alpha, score);
    }
    
    return make_tuple(best_move, best_score);
}

int main() {
    vector<int> gameboard(9, 0);
    string input;
    int move, turn, score;
    list<int> player_positions, ai_positions;
    
    cout << "Would you like to be player 1 or 2 (enter 'exit' to quit): ";
    while (true) {
        cin >> input;
        transform(input.begin(), input.end(), input.begin(), ::tolower);
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
        display_board(gameboard, player_positions, ai_positions);
        cout << endl;
        
        if (turn == 1) {
            while (true) {
                cout << "Enter move (1-9, or 'exit' to quit): ";
                cin >> input;
                transform(input.begin(), input.end(), input.begin(), ::tolower); // Make it lowercase for checks

                if (input == "exit") {
                    exit(0);
                }

                stringstream ss(input);
                if (!(ss >> move) || move < 1 || move > 9 || gameboard[move - 1] != 0) {
                    cout << "Invalid input. Please enter a number between 1 and 16 or 'exit'." << endl;
                    continue;
                }

                break;
            }
            
            move = move - 1;
            gameboard[move] = 1;
            
            // Keep track of previous moves for moveability
            player_positions.push_back(move);
            if (player_positions.size() == 4) {
                gameboard[player_positions.front()] = 0;
                player_positions.pop_front();
            }

            // Check if the human player has won
            if (check_win(gameboard, 1)) {
                display_board(gameboard, player_positions, ai_positions);
                cout << endl << "Human player wins!" << endl;
                exit(0);
            }
            
            turn = -1;
        
        } else { // Turn = -1
            
            // Get the AI move
            auto start_time = chrono::high_resolution_clock::now(); // Start measuring time
            
            tuple<int, int> result = solve(gameboard, ai_positions, player_positions, -1, 20);
            move = get<0>(result);
            score = get<1>(result);
            
            gameboard[move] = -1;
            
            // Keep track of previous moves for moveability
            ai_positions.push_back(move);
            if (ai_positions.size() == 4) {
                gameboard[ai_positions.front()] = 0;
                ai_positions.pop_front();
            }
            
            auto end_time = chrono::high_resolution_clock::now();  // Stop measuring time
            auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
            cout << "AI evaluation: " << score << endl;
            cout << "AI move took " << duration.count() << " milliseconds to calculate." << endl << endl; // Show time to calculate move

            if (check_win(gameboard, -1)) {
                display_board(gameboard, player_positions, ai_positions);
                cout << "AI player wins!" << endl;
                exit(0);
            }
            
            turn = 1;
        
        }
    }
    
    return 0;
}