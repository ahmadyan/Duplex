#pragma once
#include "optimizer.h"
#include <iostream>
using namespace std;

// The gradient descent optimization algorithm
// With support for momentum, learning rate decay and Nestrov accelerated momentum
class GradientDescent : public Optimizer{
    double learning_rate_base;
    double decay;               // the decay value of the learning rate
    double velocity;            // the velocity paramter for momentum update
    double momentum;            // dampens the velocity during momentum update
    double previous_velocity;   // previous value of velocity, used in Nestrov momentum equations.
    
    bool nestrov;               // flag that enables the Nestrov equations
    
public:
    GradientDescent(Settings* s);
    ~GradientDescent();
    bool hasGradientInformation();
    State* update(State*);
};