#pragma once
#include "optimizer.h"
#include <iostream>
using namespace std;

// The RMSProp optimization algorithm
// RMSProp is an adaptive optimization method
// Refer to "Neural Networks for Machine Learning" class notes, Lecture 6a, "Overview of mini-batch gradient descent", Slide 29, by Geoff Hinton
// available for download at http://www.cs.toronto.edu/~tijmen/csc321/slides/lecture_slides_lec6.pdf
class RMSProp : public Optimizer{
    double learning_rate_base;
public:
    RMSProp(Settings* s);
    ~RMSProp();
    bool hasGradientInformation();
    State* update(State*);
};