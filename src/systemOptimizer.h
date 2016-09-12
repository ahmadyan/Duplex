#pragma once
#include <iostream>
#include "duplex.h"
#include "nonconvexOptimizer.h"

using namespace std;

class SystemOptimizer : public NonconvexOptimizer{
public:
    SystemOptimizer(Settings* s);
    ~SystemOptimizer();
    void optimize();
    State* localStep(int i, State* qnear);
};
