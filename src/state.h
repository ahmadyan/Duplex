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
enum class StateType {StateTypeNormal, StateTypeRoot, StateTypeObjective};

class State{
    int id;
    int parentID;
    int parameterDimension;
    int objectiveDimension;
    double* parameterVector;
    double* objectiveVector;
    StateType type;
public:

    State(int, int);
    ~State();
    void setObjective(double v, int i);
    void setParameter(double* v);
    void setObjective(double* v);
    double* getParameter();
    double* getObjective();
    StateType getType();
    void setType(StateType);
	string toString();
    double unifRand();
    double unifRand(double a, double b);
    double normalRand(double mean, double std, double min, double max);
    double* uniformRandomVector(int size, double* min, double* max);
    void setParentID(int);
    int getParentID();
    void setID(int);
    int getID();
    int getObjectiveSize();
    int getParameterSize();
	double distance(State* a, double* max, double* min);
};