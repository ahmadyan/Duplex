#pragma once
#include <iostream>
#include "duplex.h"

using namespace std;

class NonconvexOptimizer: public Duplex{
public:
    NonconvexOptimizer(Settings* s);
    ~NonconvexOptimizer();
    
    double score(State* state, double* maxBound, double* minBound);
    double* getInitialState();
    void setObjective();
    
    State* initialize();
    void insert(State* s);
    State* globalStep();
    State* localStep(int, State*);
    double evaluate(State*);
    bool isConverged(int);
    
    void computeTemperature(int i);
    int computeNextCandidateParameter(State* qnear);
    double* generateNewInput(State* q);
    double computeStepLength();
};
