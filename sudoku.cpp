//
//  sudoku.cpp
//  finalproject
//
//  Created by rantd on 2014/11/29.
//  Copyright (c) 2014年 Vu Van Tan. All rights reserved.
//

#include "sudoku.h"

using namespace std;


Candidate::Candidate() {
    number.assign(9, true);
}

Candidate::~Candidate() {}

bool Candidate::check_candidate(int digit) {
    return number[digit - 1];
}
    
int Candidate::count_candidate() {
    return (int) std::count(number.begin(), number.end(), true);
}
    
void Candidate::remove_candidate(int digit) {
    number[digit - 1] = false;
}

int Candidate::get_candidate() {
    for (int i = 0; i < number.size(); ++i) {
        if (number[i]) return (i + 1);
    }
    return -1;
}

Sudoku::Sudoku() {}
Sudoku::~Sudoku() {}

Sudoku::Sudoku(vector<vector<int> > board) {
    group.resize(27);
    cells.resize(81);
    neighbors.resize(81);
    groups_of.resize(81);
    init();
    int cnt = 0;
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (board[i][j] != 0) {
                if (!assign(cnt, board[i][j])) {
                    fprintf(stderr, "invalid board\n");
                    exit(1);
                }
            }
            cnt++;
        }
    }
}
    
void Sudoku::init() {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            int k = i * 9 + j;
            int x[3] = {i, 9 + j, 18 + (i / 3) * 3 + j / 3};
            for (int g = 0; g < 3; ++g) {
                group[x[g]].push_back(k);
                groups_of[k].push_back(x[g]);
            }
        }
    }
    for (int k = 0; k < neighbors.size(); ++k) {
        for (int x = 0; x < groups_of[k].size(); ++x) {
            for (int j = 0; j < 9; j++) {
                int tk = group[groups_of[k][x]][j];
                if (tk != k) neighbors[k].push_back(tk);
            }
        }
    }
}
    
Candidate Sudoku::possible(int k) {
    return cells[k];
}
    
bool Sudoku::is_solved() {
    for (int i = 0; i < cells.size(); ++i) {
        if (cells[i].count_candidate() != 1) return false;
    }
    return true;
}
    
bool Sudoku::remove(int k, int val) {
    if (!cells[k].check_candidate(val)) return true;
    cells[k].remove_candidate(val);
    int n = cells[k].count_candidate();
    if (n == 0) return false;
    if (n == 1) {
        int cnd = cells[k].get_candidate();
        for (int i = 0; i < neighbors[k].size(); ++i) {
            if (!remove(neighbors[k][i], cnd)) return false;
        }
    }
    for (int i = 0; i < groups_of[k].size(); ++i) {
        int ind = groups_of[k][i];
        int cnt = 0, tot = -1;
        for (int j = 0; j < 9; ++j) {
            int pos = group[ind][j];
            if (cells[pos].check_candidate(val)) cnt++, tot = pos;
        }
        if (cnt == 0) return false;
        if (cnt == 1 && !assign(tot, val)) return false;
    }
    return true;
}
    
bool Sudoku::assign(int k, int val) {
    for (int digit = 1; digit <= 9; ++digit) {
        if (digit != val && !remove(k, digit)) return false;
    }
    return true;
}

int Sudoku::least_count() {
    int k = -1, min_val = 0;
    for (int i = 0; i < cells.size(); ++i) {
        int cnt = cells[i].count_candidate();
        if (cnt > 1 && (k == -1 || cnt < min_val)) min_val = cnt, k = i;
    }
    return k;
}

vector<vector<int> > Sudoku::get_result() {
    vector<vector<int> > ans(9, vector<int>(9));
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            ans[i][j] = cells[9 * i + j].get_candidate();
        }
    }
    return ans;
}

// 深さ優先探索で解を求める
Sudoku *solve(Sudoku *sdk) {
    if (sdk == NULL || sdk->is_solved()) return sdk;
    int cnt = sdk->least_count();
    Candidate cnd = sdk->possible(cnt);
    for (int digit = 1; digit <= 9; ++digit) {
        if (cnd.check_candidate(digit)) {
            Sudoku *tmp = new Sudoku(*sdk);
            if (tmp->assign(cnt, digit)) {
                Sudoku *ans = solve(tmp);
                if (ans != NULL) {
                    if (ans != tmp) delete tmp;
                    return ans;
                }
            }
            delete tmp;
        }
    }
    return NULL;
}