#pragma once
#include "state.h"

class Optimizer{
protected:
    
public:
    Optimizer();
    ~Optimizer();
    virtual bool hasGradientInformation() = 0;
    virtual State* update(State*)=0;
};