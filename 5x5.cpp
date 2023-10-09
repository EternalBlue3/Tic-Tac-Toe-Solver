#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <tuple>
#include <map>
#include <list>

using namespace std;

const vector<vector<int>> win_conditions = {{0,6,12,18},{6,12,18,24},{4,8,12,16},{8,12,16,20},{9,13,17,21},{3,7,11,15},{1,7,13,19},{5,11,17,23},{0,5,10,15},{5,10,15,20},{1,6,11,16},{6,11,16,21},{2,7,12,17},{7,12,17,22},{3,8,13,18},{8,13,18,23},{4,9,14,19},{9,14,19,24},{0,1,2,3},{1,2,3,4},{5,6,7,8},{6,7,8,9},{10,11,12,13},{11,12,13,14},{15,16,17,18},{16,17,18,19},{20,21,22,23},{21,22,23,24}};
static const array<int, 25> position_map = {0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 3, 7, 3, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0}; // Give a higher score to moves in the middle

struct TTEntry {
    int best_score;
    int depth;
    int flag;
};

typedef map<string, pair<list<string>::iterator, TTEntry>> LRUCache;
list<string> lru_list;
LRUCache table;
const int max_cache_size = 3000000; // Maximum transposition table size

void display_board(const vector<int>& gameboard) {
    for (int i = 0; i < 25; ++i) {
        char symbol = (gameboard[i] == 1) ? 'O' : ((gameboard[i] == -1) ? 'X' : ' ');
        cout << symbol;
        if ((i + 1) % 5 == 0) {
            cout << endl;
            if (i < 24) {
                cout << "-----------------" << endl;
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

int evaluate(const vector<int>& gameboard, int player) {
    int score = 0, index = 0;

    for (int cell : gameboard) {
        if (cell == player) {
            score += position_map[index];
        } else if (cell == -player) {
            score -= position_map[index];
        }
        ++index;
    }

    return score;
}

vector<int> get_possible_moves(const vector<int>& gameboard) {
    vector<int> possible_moves;
    for (int i = 0; i < 25; ++i) {
        if (gameboard[i] == 0) {
            possible_moves.push_back(i);
        }
    }
    return possible_moves;
}

void store(LRUCache& table, string board, int alpha_org, int beta, int best_score, int depth) {
    string flag;
    if (best_score <= alpha_org) {
        flag = "UPPERCASE";
    } else if (best_score >= beta) {
        flag = "LOWERCASE";
    } else {
        flag = "EXACT";
    }

    TTEntry entry = {best_score, depth, (flag == "EXACT" ? 0 : (flag == "LOWERCASE" ? -1 : 1))};

    if (table.size() >= max_cache_size) {
        string to_evict = lru_list.back();
        lru_list.pop_back();
        table.erase(to_evict);
    }

    lru_list.push_front(board);
    table[board] = make_pair(lru_list.begin(), entry);
}

int negamax(vector<int> gameboard, int player, int depth, int alpha, int beta, LRUCache& TT) {
    int alpha_org = alpha;

    // Transposition table lookup
    string board_str;
    for (int i : gameboard) {
        board_str += to_string(i);
    }

    if (TT.find(board_str) != TT.end()) {
        // Get TT data
        TTEntry tt_entry = TT[board_str].second;
        int tt_value = tt_entry.best_score;
        int tt_depth = tt_entry.depth;
        int tt_flag = tt_entry.flag;

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
        return 100+depth;
    }

    if (check_win(gameboard, -player)) {
        return -100-depth;
    }

    if (find(gameboard.begin(), gameboard.end(), 0) == gameboard.end()) {
        return 0;
    }

    if (depth == 0) {
        return evaluate(gameboard, player);
    }

    int best_score = -10000; // Initial best score
    int score;

    for (int move : get_possible_moves(gameboard)) {
        gameboard[move] = player;

        // Use a null window search by calling negamax with a narrow window
        score = -negamax(gameboard, -player, depth-1, -alpha-1, -alpha, TT);

        // If the score is inside the new window, re-evaluate with a proper window
        if (alpha < score && score < beta) {
            score = -negamax(gameboard, -player, depth-1, -beta, -score, TT);
        }

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

tuple<int, int> solve(vector<int> gameboard, int player, int max_depth) {
    int score, best_move, best_score, alpha, beta;

    LRUCache TT;

    auto start_time = chrono::high_resolution_clock::now();
    auto end_time = chrono::high_resolution_clock::now();
    auto max_duration = chrono::milliseconds(1000); // Maximum search time (adjust as needed)

    for (int depth = 1; depth <= max_depth; depth++) {
        best_score = -10000; // Reset best_score for this depth
        alpha = -10000; // Reset alpha value
        beta = 10000; // Reset beta value

        for (int move : get_possible_moves(gameboard)) {
            gameboard[move] = player;
            score = -negamax(gameboard, -player, depth - 1, -beta, -alpha, TT);
            gameboard[move] = 0;

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }

            alpha = max(alpha, score);
        }

        end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

        cout << "Searching at depth: " << to_string(depth) << "\r" << flush;

        if (duration >= max_duration) {
            break; // Exit the loop if the time limit is reached
        }
    }

    return make_tuple(best_move, best_score);
}

int main() {
    vector<int> gameboard(25, 0);
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
                cout << "Enter move (1-25, or 'exit' to quit): ";
                cin >> input;
                transform(input.begin(), input.end(), input.begin(), ::tolower); // Make it lowercase for checks

                if (input == "exit") {
                    exit(0);
                }

                stringstream ss(input);
                if (!(ss >> move) || move < 1 || move > 25 || gameboard[move - 1] != 0) {
                    cout << "Invalid input. Please enter a number between 1 and 25 or 'exit'." << endl;
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

            tuple<int, int> result = solve(gameboard, -1, 25);
            move = get<0>(result);
            score = get<1>(result);

            gameboard[move] = -1;
            auto end_time = chrono::high_resolution_clock::now();  // Stop measuring time
            auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
            cout << "AI evaluation: " << score << "          " << endl; // Add white space to cover up printed depth
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
