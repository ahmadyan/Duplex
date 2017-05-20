#pragma once
#include "optimizer.h"
#include <iostream>
using namespace std;

// The Adam optimization algorithm
// See: Kingma-Ba: Adam: A method for stochastic optimization, ICLR-2015
// http://arxiv.org/abs/1412.6980v8
class Adam : public Optimizer {
  double learning_rate_base;
  // an epsilon to avoid division by zero (1e-4 to 1e-8)
  double fudgeFactor;
  // Exponential decay rates for the moment estimates, 0<beta<1, generally close
  // to 1
  double exponential_decay_rates_beta1;
  double exponential_decay_rates_beta2;
  int iterations;
  double *moment;
  double *velocity;
  double *u;
  double decay_beta1;
  double decay_beta2;
  bool bias_correction;

  double *prev;
  vector<double> dx;
  bool adamax;

public:
  Adam(Settings *s);
  ~Adam();
  bool hasGradientInformation();
  State *update(State *);
  void update_adam(double *);
  void update_adamax(double *);
};