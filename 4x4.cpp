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

const vector<vector<int>> win_conditions = {{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15},{0,4,8,12},{1,5,9,13},{2,6,10,14},{3,7,11,15},{0,5,10,15},{3,6,9,12}, {0,3,12,15}, {0,1,4,5}, {1,2,5,6}, {2,3,6,7}, {4,5,8,9}, {5,6,9,10}, {6,7,10,11}, {8,9,12,13}, {9,10,13,14}, {10,11,14,15}}; // https://mamabeefromthehive.blogspot.com/2012/01/4-square-tic-tac-toe.html for more info on 4x4 rules

vector<tuple<string, int, int>> load_dictionary() {
    vector<tuple<string, int, int>> dictionary;
    string board, row;
    int move, score;
    
    ifstream file("dictionary.txt");
    if (file.is_open()) {
        while (getline(file, row)) {
            if (file >> board >> move >> score) {
                dictionary.push_back(make_tuple(board, move, score));
            } else {
                cerr << "Error parsing row: " << row << endl;
            }
        }
    } else {
        cerr << "Unable to locate game dictionary." << endl;
        exit(1);
    }
    file.close();
    
    return dictionary;
}

void display_board(const vector<int>& gameboard) {
    for (int i = 0; i < 16; ++i) {
        char symbol = (gameboard[i] == 1) ? 'O' : ((gameboard[i] == -1) ? 'X' : ' ');
        cout << symbol;
        if ((i + 1) % 4 == 0) {
            cout << endl;
            if (i < 15) {
                cout << "-------------" << endl;
            }
        } else {
            cout << " | ";
        }
    }
}

bool check_win(const vector<int>& gameboard, const int& player) {
    for (const auto& condition : win_conditions) {
        if (gameboard[condition[0]] == player && gameboard[condition[1]] == player && gameboard[condition[2]] == player && gameboard[condition[3]] == player) {
            return true;
        }
    }

    return false;
}

vector<int> get_possible_moves(const vector<int>& gameboard) {
    vector<int> possible_moves;
    for (int i = 0; i < 16; ++i) {
        if (gameboard[i] == 0) {
            possible_moves.push_back(i);
        }
    }
    return possible_moves;
}

void store(map<string, vector<int>>& table, string board, int alpha, int beta, int best_score, int depth) {
    string flag;
    if (best_score <= alpha) {
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
        int tt_alpha = tt_entry[0];
        int tt_beta = tt_entry[1];
        int tt_flag = tt_entry[2];

        if (tt_flag == 0) {
            return tt_alpha;
        } else if (tt_flag == -1) {
            alpha = max(alpha, tt_beta);
        } else if (tt_flag == 1) {
            beta = min(beta, tt_alpha);
        }

        if (alpha >= beta) {
            return tt_alpha;
        }
    }
    
    // Terminal node checks
    if (check_win(gameboard, player)) {
        return 100 + depth;
    }
    
    if (check_win(gameboard, -player)) {
        return -100 - depth;
    }
    
    if (find(gameboard.begin(), gameboard.end(), 0) == gameboard.end()) {
        return 0;
    }
    
    if (depth == 0) {
        return 0;
    }
    
    int best_score = -1000;
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
    int best_score = -1000;
    int alpha = -1000; // Initial alpha value
    int beta = 1000;   // Initial beta value
    
    map<string, vector<int>> TT;
    
    for (int move : get_possible_moves(gameboard)) {
        gameboard[move] = player;
        score = -negamax(gameboard, -player, depth-1, -beta, -alpha, TT);
        gameboard[move] = 0;
        
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
        
        alpha = max(alpha, score);
        
        if (alpha >= beta) {
            break;
        }
    }
    
    return make_tuple(best_move, best_score);
}

int main() {
    vector<int> gameboard(16, 0);
    string input;
    int move, turn, score;
    bool found;
    
    int moves_made = 0;
    vector<tuple<string, int, int>> dictionary = load_dictionary();
    
    // Rest of code
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
                cout << "Enter move (1-16, or 'exit' to quit): ";
                cin >> input;
                transform(input.begin(), input.end(), input.begin(), ::tolower); // Make it lowercase for checks

                if (input == "exit") {
                    exit(0);
                }

                stringstream ss(input);
                if (!(ss >> move) || move < 1 || move > 16 || gameboard[move - 1] != 0) {
                    cout << "Invalid input. Please enter a number between 1 and 16 or 'exit'." << endl;
                    continue;
                }

                gameboard[move - 1] = 1;
                moves_made++;
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
            if (moves_made <= 4) {
                string board_str;
                for (int i : gameboard) {
                    board_str += to_string(i);
                }

                found = false;
                for (const auto& entry : dictionary) {
                    if (get<0>(entry) == board_str) {
                        move = get<1>(entry);
                        score = get<2>(entry);
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    cerr << "Error: Board state not found in dictionary." << endl;
                    cout << board_str;
                    exit(1);
                }
            } else {
                tuple<int, int> result = solve(gameboard, -1, 16);
                move = get<0>(result);
                score = get<1>(result);
            }
            
            gameboard[move] = -1;
            moves_made++;
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
