#pragma once
#include <iostream>
#include "duplex.h"

using namespace std;

class FunctionalOptimizer{
public:
    FunctionalOptimizer(Settings* s);
    ~FunctionalOptimizer();
    
    double* getInitialState();
    void setObjective();
    
    State* initialize();
    void insert(State* s);
    State* globalStep();
    State* localStep(int, State*);
    double evaluate(State*);
    bool isConverged(int, State*);
    
    void computeTemperature(int i);
    int computeNextCandidateParameter(State* qnear);
    double* generateNewInput(State* q);
    double computeStepLength();
};