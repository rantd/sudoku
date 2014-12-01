//
//  sudoku.h
//  finalproject
//
//  Created by rantd on 2014/11/29.
//  Copyright (c) 2014年 Vu Van Tan. All rights reserved.
//

#ifndef _sudoku_h
#define _sudoku_h

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>

using namespace std;

class Candidate {
private:
    vector<bool> number;
public:
    Candidate();
    ~Candidate();
    bool check_candidate(int digit);
    int count_candidate();
    void remove_candidate(int digit);
    int get_candidate();
};

class Sudoku {
private:
    vector<Candidate> cells;
    vector<vector<int> > group, neighbors, groups_of;
public:
    Sudoku();
    ~Sudoku();
    Sudoku(vector<vector<int> > board);
    void init();
    Candidate possible(int k);
    bool is_solved();
    bool remove(int k, int val);
    bool assign(int k, int val);
    int least_count();
    vector<vector<int> > get_result();
};

Sudoku *solve(Sudoku *sdk);
#endif
