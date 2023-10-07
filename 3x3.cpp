#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <tuple>
#include <map>
#include <fstream>

using namespace std;

const vector<vector<int>> win_conditions = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};

void display_board(const vector<int>& gameboard) {
    for (int i = 0; i < 9; ++i) {
        char symbol = (gameboard[i] == 1) ? 'O' : ((gameboard[i] == -1) ? 'X' : ' ');
        cout << symbol;
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

void store(map<string, vector<int>>& table, string board, int alpha_org, int beta, int best_score, int depth) {
    string flag;
    if (best_score <= alpha_org) {
        flag = "UPPERCASE";
    } else if (best_score >= beta) {
        flag = "LOWERCASE";
    } else {
        flag = "EXACT";
    }

    vector<int> entry = {best_score, depth};
    entry.push_back(flag == "EXACT" ? 0 : (flag == "LOWERCASE" ? -1 : 1));
    table[board] = entry;
}

int negamax(vector<int> gameboard, int player, int depth, int alpha, int beta, map<string, vector<int>>& TT) {
    int alpha_org = alpha;

    // Transposition table lookup
    string board_str;
    for (int i : gameboard) {
        board_str += to_string(i);
    }

    if (TT.find(board_str) != TT.end()) {
        // Get TT data
        vector<int> tt_entry = TT[board_str];
        int tt_value = tt_entry[0];
        int tt_depth = tt_entry[1];
        int tt_flag = tt_entry[2];
        
        if (tt_depth >= depth) {

            if (tt_flag == 0) {
                return tt_value;
            } else if (tt_flag == -1) {
                alpha = max(alpha, tt_value);
            } else if (tt_flag == 1) {
                beta = min(beta, tt_value);
            }

            if (alpha >= beta) {
                return tt_value;
            }
        }
    }
    
    // Terminal node checks
    if (check_win(gameboard, player)) {
        return depth;
    }
    
    if (check_win(gameboard, -player)) {
        return -depth;
    }
    
    if (find(gameboard.begin(), gameboard.end(), 0) == gameboard.end()) {
        return 0;
    }
    
    if (depth == 0) {
        return 0;
    }
    
    int best_score = -10000; // Initial best score
    int score;
    
    for (int move : get_possible_moves(gameboard)) {
        gameboard[move] = player;
        score = -negamax(gameboard, -player, depth-1, -beta, -alpha, TT);
        gameboard[move] = 0;
                
        if (score > best_score) {
            best_score = score;
        }
        
        alpha = max(alpha, score);
        
        if (alpha >= beta) {
            break;
        }
    }
    
    store(TT, board_str, alpha_org, beta, best_score, depth);
    
    return best_score;
}

tuple<int, int> solve(vector<int> gameboard, int player, int depth) {
    int best_move, score;
    int best_score = -10000; // Initial best score
    int alpha = -10000; // Initial alpha value
    int beta = 10000;   // Initial beta value
    
    map<string, vector<int>> TT;
    
    for (int move : get_possible_moves(gameboard)) {
        gameboard[move] = player;
        score = -negamax(gameboard, -player, depth-1, -beta, -alpha, TT);
        gameboard[move] = 0;
                
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }
    
    return make_tuple(best_move, best_score);
}

int main() {
    vector<int> gameboard(9, 0);
    string input;
    int move, turn, score;
    
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
        display_board(gameboard);
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

            if (find(gameboard.begin(), gameboard.end(), 0) == gameboard.end()) {
                display_board(gameboard);
                cout << endl << "Game was a draw." << endl;
                exit(0);
            }
            
            turn = -1;
        
        } else { // Turn = -1
            
            // Get the AI move
            auto start_time = chrono::high_resolution_clock::now(); // Start measuring time
            
            tuple<int, int> result = solve(gameboard, -1, 9);
            move = get<0>(result);
            score = get<1>(result);
            
            gameboard[move] = -1;
            auto end_time = chrono::high_resolution_clock::now();  // Stop measuring time
            auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
            cout << "AI evaluation: " << score << endl;
            cout << "AI move took " << duration.count() << " milliseconds to calculate." << endl << endl; // Show time to calculate move

            if (check_win(gameboard, -1)) {
                display_board(gameboard);
                cout << "AI player wins!" << endl;
                exit(0);
            }

            if (find(gameboard.begin(), gameboard.end(), 0) == gameboard.end()) {
                display_board(gameboard);
                cout << endl << "Game was a draw." << endl;
                exit(0);
            }
            
            turn = 1;
        
        }
    }
    
    return 0;
}
