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
    Configuration* config;
    State* root;
    State* goal;
    System* system;
    Search* db;
public:
    Duplex(Configuration*);
    ~Duplex();
    void initialize(double*);
    void setObjective(double*);
    State* generateSample();
    void optimize();
    void setSystem(System*);
    double distance();
};