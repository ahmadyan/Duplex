#pragma once
#include <iostream>
#include "duplex.h"

using namespace std;

class NonconvexOptimizer: public Duplex{
public:
    NonconvexOptimizer(Settings* s);
    ~NonconvexOptimizer();
    
    double* getInitialState();
    void setObjective();
    
    void initialize();
    State* globalStep();
    State* localStep(int, State*);
    double evaluate(State*);
    bool isConverged(int, State*);
    
    void computeTemperature(int i);
    int computeNextCandidateParameter(State* qnear);
    double* generateNewInput(State* q);
    double computeStepLength();
    void setup();
	void post();
};
