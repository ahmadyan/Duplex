#pragma once
#include "optimizer.h"
#include <iostream>
using namespace std;

// The Adagrad optimization algorithm
class Adagrad : public Optimizer{
    double learning_rate_base;
    int parameterDimension;
    int objectiveDimension;
    
    // an epsilon to avoid division by zero (1e-4 to 1e-8)
    double fudgeFactor;
    
    // keeps track of per-parameter sum of squared gradients.
    // Normalizes the update step, element-wise
    double* cache;
    
public:
    Adagrad(Settings* s);
    ~Adagrad();
    bool hasGradientInformation();
    State* update(State*);
};