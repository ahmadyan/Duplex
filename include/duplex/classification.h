#pragma once
#include "configuration.h"
#include "data.h"
#include "duplex.h"
#include "nonconvexOptimizer.h"
#include "optimizer.h"
#include <iostream>

using namespace std;

class Classifier : public NonconvexOptimizer {
  Data *data;
  Optimizer *optimizer;
  Settings *settings;
  int featureSize;
  int classSize;
  int sampleSize;

public:
  Classifier(Settings *, Data *);
  ~Classifier();
  double *getInitialState();
  State *localStep(int, State *);
  double evaluate(State *);
  double *loss(double *theta);
  double sigmoid(double x);
  double dot(double *theta, vector<double>);
  double logistic(double *theta, vector<double>);
  double regularizer(double *theta);
  double *gradient(double *theta);
  double score(State *state, double *maxBound, double *minBound);
};
