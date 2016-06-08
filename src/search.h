//
//  search.h
//  duplex
//
//  Created by Adel Ahmadyan on 5/5/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <boost/property_tree/ptree.hpp>
#include "state.h"
#include "kdtree.h"
#include "configuration.h"
using namespace std;

class Search{
    Settings* config;
    int dim;
    vector<State*> db;
    struct kdtree *kd;
	priority_queue<State*, vector<State*>, stateGreaterComparator > pq;
public:
	Search(Settings*);
	~Search();
    //State operator[] (const int i);
    State* getState(int i);
    State* getState();
    void setState(State* s, int i);
    void insert(State*);
	State* getOptimum();
    State* nearestNode(State*);
    void range(State*);
    unsigned long getSize();
    void save(boost::property_tree::ptree*);
    void load(boost::property_tree::ptree*);
};