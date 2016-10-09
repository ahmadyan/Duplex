#pragma once
#include <iostream>
#include "duplex.h"

using namespace std;

class RRT: public Duplex{
public:
    RRT(Settings* s);
    ~RRT();
    
    double* getInitialState();
    void setObjective();
    
    void initialize();
    State* globalStep();
    State* localStep(int, State*);
    double evaluate(State*);
    bool isConverged(int, State*);
};
