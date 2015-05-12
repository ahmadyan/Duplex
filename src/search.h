//
//  search.h
//  duplex
//
//  Created by Adel Ahmadyan on 5/5/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once
#include "state.h"
#include "kdtree.h"
#include <vector>

class Search{
    int dim;
    vector<State*> db;
    struct kdtree *kd;
public:
	Search(int);
	~Search();
    //State operator[] (const int i);
    State* getState(int i);
    void setState(State* s, int i);
    void insert(State*);
    State* search(State*);
    void range(State*);
    unsigned long getSize();
};