#pragma once
#include "optimizer.h"
#include <iostream>
using namespace std;

// The Adagrad optimization algorithm, with support for RMSProp auto-correlation
class Adagrad : public Optimizer{
    double learning_rate_base;
    // an epsilon to avoid division by zero (1e-4 to 1e-8)
    double fudgeFactor;
    bool init;
    // keeps track of per-parameter sum of squared gradients.
    // Normalizes the update step, element-wise
    double* cache;
    // autocorr is the decay rate in the RMSProp algorithm.
    double autocorr;
    
public:
    Adagrad(Settings* s);
    ~Adagrad();
    bool hasGradientInformation();
    State* update(State*);
};