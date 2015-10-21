//
//  duplex.h
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once
#include "configuration.h"
#include "system.h"
#include "search.h"

class Duplex{
    int parameterDimension;
    int objectiveDimension;
	Settings* settings;
    State* root;
    State* goal;
    System* system;
    Search* db;

	//keeps the minimum distance from any node in the tree toward the optimum point
	//should eventually converge to zero
	vector<double> error;
public:
	Duplex(Settings*);
    ~Duplex();
    void initialize(double*);
    void setObjective(double*);
    State* generateSample();
    void optimize();
    void setSystem(System*);
    double distance();
    string draw();
    double* generateNewInput(State* q, double temperature);
    string drawParameterTree();
    string drawObjectiveTree();
	void updateError(State* s, double* max, double* min);
	string plotError();
};