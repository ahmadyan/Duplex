#pragma once
#include "configuration.h"
#include "state.h"

class Optimizer {
protected:
  Settings *settings;
  int parameterDimension;
  int objectiveDimension;

public:
  Optimizer(Settings *setting);
  ~Optimizer();
  void clipParameters(double *);
  virtual bool hasGradientInformation() = 0;
  virtual State *update(State *) = 0;
};