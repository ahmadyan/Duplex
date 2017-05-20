#pragma once
#include "configuration.h"
#include "sensitivity.h"
#include "state.h"

class Stat {
  Settings *settings;

  double *max;
  double *min;
  double *opt;
  double minAward;
  double maxAward;
  double delta;
  bool reinforcementLearningOption;
  bool shrinkGoalRegionWithTemperateOption;
  int nextCandidateParameter;
  vector<string> objectiveType;

  vector<double> currentDistance;
  vector<double> error; // keeps the minimum distance from any node in the tree
                        // toward the optimum point (Hopefully converges to 0)

  vector<State *> bias;
  vector<double> convergence;

public:
  Sensitivity *sensitivity;

  Stat(Settings *, double *, double *, double *);
  ~Stat();

  void update(int i, State *goal, State *qsample, State *qnear, State *qnew,
              double);

  void updateSensitivity(State *qnear, State *qnew);
  void updateReward(State *, State *qnear, State *qnew);
  void updateError(double);
  void updateBias(State *q);
  void updateConvergence(State *q);

  double getDeltaConvergence();
  double getError(int);
  double getDistance(int);
  void error_push_back(double v);
  void distance_push_back(double v);
  void push_back(double);
  vector<double> getError();
  vector<double> getCurrentDistance();
};
