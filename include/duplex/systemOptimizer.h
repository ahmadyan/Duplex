#pragma once
#include "duplex.h"
#include "nonconvexOptimizer.h"
#include <iostream>

using namespace std;

class SystemOptimizer : public NonconvexOptimizer {
public:
  SystemOptimizer(Settings *s);
  ~SystemOptimizer();
  void optimize();
  State *localStep(int i, State *qnear);
  void setup();
  void initialize();
  void post();
};
