#pragma once
#include "optimizer.h"
#include <iostream>
using namespace std;

// The AdaDelta optimization algorithm
// see: Matthew D. Zeiler, Adadelta: an adaptive learning rate method [http://arxiv.org/pdf/1212.5701v1.pdf]
class Adadelta : public Optimizer{
    double learning_rate_base;
    double fudge_factor;
    double autocorrelation;
    double* accumulator;
    double* delta;
    double* step; //update vector
    bool accelerated_adaDelta;

public:
    Adadelta(Settings* s);
    ~Adadelta();
    bool hasGradientInformation();
    State* update(State*);
};
