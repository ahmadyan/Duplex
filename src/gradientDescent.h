#pragma once
#include "optimizer.h"
#include <iostream>
using namespace std;

class GradientDescent : public Optimizer{
    double learning_rate;
    double decay;
    double velocity;
    double momentum;
public:
    GradientDescent(Settings* s);
    ~GradientDescent();
    bool hasGradientInformation();
    State* update(State*);
};