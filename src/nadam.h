#pragma once
#include "optimizer.h"
#include <iostream>
using namespace std;

// The Nestrov+Adam optimization algorithm, Nadam is Adam RMSprop with Nesterov momentum.
// See Nadam report : http://cs229.stanford.edu/proj2015/054_report.pdf
class Nadam : public Optimizer{
    double learning_rate_base;
    // an epsilon to avoid division by zero (1e-4 to 1e-8, default=1e-8)
    double fudgeFactor;
    // Exponential decay rates for the moment estimates, 0<beta<1, generally close to 1
    double exponential_decay_rates_beta1;
    double exponential_decay_rates_beta2;
    int iterations;
    double* moment;
    double* velocity;
    double m_schedule;
    double decay_beta1; //default=0.9
    double decay_beta2; //default=0.999
    double schedule_decay; //default=0.004
    double* prev;
    vector<double> dx;
public:
    Nadam(Settings* s);
    ~Nadam();
    bool hasGradientInformation();
    State* update(State*);
};