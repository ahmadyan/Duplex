#include "adadelta.h"
#include <cmath>

Adadelta::Adadelta(Settings *s) : Optimizer(s) {
  learning_rate_base = settings->lookupFloat("optimization.learning_rate");
  accelerated_adaDelta =
      settings->check("optimization.adadelta_acceleration", "enable");
  fudge_factor = settings->lookupFloat("optimization.fudge_factor");
  ;
  autocorrelation = settings->lookupFloat("optimization.autocorrelation");
  cout << "Initializing Adadelta optimizer complete"
       << ", learning rate = None"
       << ", fudge_factor=" << fudge_factor
       << ", autocorrelation (rho)=" << autocorrelation
       << ", accelerated=" << accelerated_adaDelta << endl;
  accumulator = new double[parameterDimension]();
  delta = new double[parameterDimension]();
  step = new double[parameterDimension]();
}

Adadelta::~Adadelta() {}

bool Adadelta::hasGradientInformation() { return true; }

State *Adadelta::update(State *u) {
  auto prev = u->getParameter();
  auto v = new State(parameterDimension, objectiveDimension);
  auto input = new double[parameterDimension]();
  auto learningRate = learning_rate_base;
  auto dx = u->getDerivativeVector(0);

  for (int i = 0; i < parameterDimension; i++) {
    // accumulate the gradient
    accumulator[i] = autocorrelation * accumulator[i] +
                     (1 - autocorrelation) * dx[i] * dx[i];
    // use the new accumulator and the *old* delta_accumulator
    step[i] = dx[i] * sqrt(delta[i] + fudge_factor) /
              sqrt(accumulator[i] + fudge_factor);
    // update delta accumulator
    if (accelerated_adaDelta) {
      // using the gradient information to make bigger jumps
      delta[i] =
          autocorrelation * delta[i] + (1 - autocorrelation) * step[i] * dx[i];
    } else {
      // original adadelta equation:
      delta[i] = autocorrelation * delta[i] +
                 (1 - autocorrelation) * step[i] * step[i];
    }
    // apply update
    input[i] = prev[i] - step[i];
  }

  clipParameters(input);
  v->setParameter(input);
  return v;
}
