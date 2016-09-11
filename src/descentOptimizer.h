#pragma once
#include <iostream>
#include "duplex.h"
#include "optimizer.h"

using namespace std;

// Descentoptimizer, a generic term for descent algorithms, such as gradient descent, Adam, RMSProp, etc.
class DescentOptimizer: public Duplex{
    Optimizer* optimizer;
public:
    DescentOptimizer(Settings* s);
    ~DescentOptimizer();

    double* getInitialState();
    void setObjective();
    void initialize();
    State* globalStep();
    State* localStep(int, State*);
    double evaluate(State*);
    bool isConverged(int, State*);
};
