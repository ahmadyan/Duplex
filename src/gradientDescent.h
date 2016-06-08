#pragma once
#include "optimizer.h"
#include <iostream>
using namespace std;

class GradientDescent : public Optimizer{
public:
    GradientDescent();
    ~GradientDescent();
    bool hasGradientInformation();
    void optimize();
};