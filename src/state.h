//
//  state.h
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once
#include <vector>
#include "configuration.h"

using namespace std;
class State{
    int id;
    int parameterDimension;
    int objectiveDimension;
    double* parameterVector;
    double* objectiveVector;
public:
    State(int, int);
    ~State();
    void setParameter(double*);
    void setObjective(double*);
    double* getParameter();
    double* getObjective();
};