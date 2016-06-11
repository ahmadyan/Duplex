#pragma once
#include "state.h"
#include "configuration.h"

class Optimizer{
protected:
    Settings* settings;
public:
    Optimizer(Settings* setting);
    ~Optimizer();
    virtual bool hasGradientInformation() = 0;
    virtual State* update(State*)=0;
};