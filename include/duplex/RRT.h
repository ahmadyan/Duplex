#pragma once
#include "duplex.h"
#include <fstream>
#include <iostream>
using namespace std;

class RRT : public Duplex {
public:
  RRT(Settings *s);
  ~RRT();
  ofstream myfile;
  double *getInitialState();
  void setObjective();

  void initialize();
  State *globalStep();
  State *localStep(int, State *);
  double evaluate(State *);
  bool isConverged(int, State *);
  State *NearestState(State *);
  // double* RRT::generateNewInput(State* q);
  double two_points_distance(State *, State *);
  void writeTofile(State *, State *);
};
