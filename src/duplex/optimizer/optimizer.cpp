#include "optimizer.h"

Optimizer::Optimizer(Settings *s) {
  settings = s;
  parameterDimension = settings->lookupInt("parameter.size");
  objectiveDimension = settings->lookupInt("objective.size");
}

Optimizer::~Optimizer() {}

void Optimizer::clipParameters(double *input) {
  for (int i = 0; i < parameterDimension; i++) {
    if (input[i] < -1)
      input[i] = -1;
    if (input[i] > 1)
      input[i] = 1;
  }
}