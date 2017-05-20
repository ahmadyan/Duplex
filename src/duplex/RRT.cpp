#include "RRT.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <stdlib.h> /* srand, rand */
#include <time.h>

RRT::RRT(Settings *s) : Duplex(s) { engine = "rrt"; }

RRT::~RRT() {}

double *RRT::getInitialState() {
  srand(time(NULL));
  double *init = new double[parameterDimension];
  bool initialStateAssignmentIsRandom =
      settings->check("initial_state_assignment", "random");

  vector<string> parameters =
      settings->listVariables("parameter", "uid-parameter");
  for (int i = 0; i < parameters.size(); i++) {
    if (initialStateAssignmentIsRandom) {
      double min = settings->lookupFloat(
          ("parameter." + parameters[i] + ".range.min").c_str());
      double max = settings->lookupFloat(
          ("parameter." + parameters[i] + ".range.max").c_str());
      init[i] = (max - min) * ((1.0 * rand()) / RAND_MAX) + min;
    } else {
      init[i] = settings->lookupFloat(
          ("parameter." + parameters[i] + ".init").c_str());
    }
  }
  return init;
}

void RRT::setObjective() {
  goal = new State(parameterDimension, objectiveDimension);
  vector<string> objectives =
      settings->listVariables("objective", "uid-objective");
  for (int i = 0; i < objectiveDimension; i++) {
    string val = settings->lookupString(
        ("objective." + objectives[i] + ".goal.optimum").c_str());
    goal->setObjective(stod(val), i);
  }
}

void RRT::initialize() {
  setObjective();
  cout << "Duplex initialization started. It make take a while to analyze the "
          "root."
       << endl;
  double *init = getInitialState();
  double *reward = new double[parameterDimension];

  for (int i = 0; i < parameterDimension; i++)
    reward[i] = 1;

  // setting boundaries for the objectives
  objectiveType = settings->listValues("objective", "uid-objective.goal.mode");
  vector<string> objectiveGoalMinStringVector =
      settings->listValues("objective", "uid-objective.goal.min");
  vector<string> objectiveGoalMaxStringVector =
      settings->listValues("objective", "uid-objective.goal.max");
  vector<string> objectiveMinStringVector =
      settings->listValues("objective", "uid-objective.min");
  vector<string> objectiveMaxStringVector =
      settings->listValues("objective", "uid-objective.max");
  vector<string> objectiveOptimumStringVector =
      settings->listValues("objective", "uid-objective.goal.optimum");
  goalRegionBoxMin = new double[objectiveDimension];
  goalRegionBoxMax = new double[objectiveDimension];
  max = new double[objectiveDimension];
  min = new double[objectiveDimension];
  opt = new double[objectiveDimension];
  for (int i = 0; i < objectiveDimension; i++) {
    goalRegionBoxMin[i] = stod(objectiveGoalMinStringVector[i]);
    goalRegionBoxMax[i] = stod(objectiveGoalMaxStringVector[i]);
    min[i] = stod(objectiveMinStringVector[i]);
    max[i] = stod(objectiveMaxStringVector[i]);
    opt[i] = stod(objectiveOptimumStringVector[i]);
  }
  cout << "Goals are set." << endl;

  vector<string> parametersMinStringVector =
      settings->listValues("parameter", "uid-parameter.range.min");
  vector<string> parametersMaxStringVector =
      settings->listValues("parameter", "uid-parameter.range.max");
  parameterMin = new double[parameterDimension];
  parameterMax = new double[parameterDimension];
  for (int i = 0; i < parameterDimension; i++) {
    parameterMin[i] = stod(parametersMinStringVector[i]);
    parameterMax[i] = stod(parametersMaxStringVector[i]);
  }

  stat = new Stat(settings, max, min, opt);
  cout << "Statistic class initialized." << endl;

  // Setting up the root node
  State *root = new State(parameterDimension, objectiveDimension);
  root->setParameter(init);
  root->setReward(reward, (double)parameterDimension);
  root->setID(0);
  root->setParentID(-1);
  system->eval(root);
  double distance = score(root, max, min);
  stat->updateError(distance);
  db->insert(root);
  cout << "Root node set." << endl;
  // myfile.open("C:\\MyGit\\RRTSearch\\bin\\mydata.txt");
  myfile.open("RRTtreedata.txt");
}

State *RRT::globalStep() {
  double delta = initialStepLength;
  State *qsample = new State(parameterDimension, objectiveDimension);
  // qsample->setParameter(qsample->uniformRandomVector(parameterDimension,
  // goalRegionBoxMin, goalRegionBoxMax));
  auto randomstate = qsample->uniformRandomVector(
      objectiveDimension, goalRegionBoxMin, goalRegionBoxMax);
  qsample->setParameter(randomstate);
  qsample->setObjective(randomstate);
  auto qprev = NearestState(qsample);

  double *qsampleobj = qsample->getObjective();
  double *qprevobj = qprev->getObjective();
  /*if (two_points_distance(qsample, qprev) < delta) {
          qsample->setID(db->getSize() );
          qsample->setParent(qprev);
          db->insert(qsample);
          //writeTofile(qprev, qsample);
          return qsample;
  }
  else {*/
  double theta =
      atan2(qsampleobj[1] - qprevobj[1], qsampleobj[0] - qprevobj[0]);
  State *qnew = new State(parameterDimension, objectiveDimension);
  double *input = new double[2];
  input[0] = qprevobj[0] + delta * cos(theta) * rand() / double(RAND_MAX);
  input[1] = qprevobj[1] + delta * sin(theta) * rand() / double(RAND_MAX);
  qnew->setParameter(input);
  qnew->setID(db->getSize());
  qnew->setParent(qprev);
  db->insert(qnew);
  writeTofile(qprev, qnew);
  return qnew;
  //}
}
State *RRT::NearestState(State *qrand) {
  double best = 2 * parameterMax[1];
  // double* init = getInitialState();
  auto bestnode = db->getState();
  // State* bestnode = new State(parameterDimension, objectiveDimension);
  /*bestnode->setObjective(init);
  bestnode->setParameter(init);*/
  for (int i = 1; i < db->getSize(); ++i) {
    auto test = db->getState(i);
    double dist = two_points_distance(test, qrand);
    if (dist < best) {
      best = dist;
      bestnode = test;
    }
  }
  return bestnode;
}
double RRT::two_points_distance(State *p, State *q) {
  int osize = q->getObjectiveSize();
  auto qobj = q->getObjective();
  auto pobj = p->getObjective();
  double dis = 0;
  for (int i = 0; i < osize; ++i) {
    dis += pow((pobj[i] - qobj[i]), 2);
  }
  dis = sqrt(dis);
  return dis;
}
State *RRT::localStep(int i, State *qnear) { return qnear; }

double RRT::evaluate(State *qnew) {
  system->eval(qnew, 0); // simulate the circuit with the
  score(qnew, max, min);
  return qnew->getScore();
}

bool RRT::isConverged(int iteration, State *q) {
  /*bool inDestination;
  auto qlatest = db->getState();
  double* qlatetpara = qlatest->getParameter();
  inDestination = (qlatetpara[0] > goalRegionBoxMin[0]) && (qlatetpara[1] >
  goalRegionBoxMin[1]) && (qlatetpara[0] < goalRegionBoxMax[0]) &&
  (qlatetpara[1] < goalRegionBoxMax[1]);*/
  bool inDestination = false;
  bool out = (iteration < iterationCap) || inDestination;
  if (out) {
    return out;
  } else {
    myfile.close();
    return out;
  }
}

void RRT::writeTofile(State *qprev, State *qnew) {
  double *qprevobj = qprev->getParameter();
  double *qnewobj = qnew->getParameter();
  myfile << qprevobj[0] << " " << qprevobj[1] << " " << qnewobj[0] << " "
         << qnewobj[1] << endl;
}