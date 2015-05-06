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
class Duplex{
    int parameterDimension;
    int objectiveDimension;
    Configuration* config;
    
public:
    Duplex(Configuration*);
    void setInitialState();
    void setObjectiveState();
    void optimize();
    void setSystem(System*);
    ~Duplex();
};