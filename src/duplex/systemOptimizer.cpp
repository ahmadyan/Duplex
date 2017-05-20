#include "systemOptimizer.h"

SystemOptimizer::SystemOptimizer(Settings *s) : NonconvexOptimizer(s) {
  engine = "system-optimizer";
}

SystemOptimizer::~SystemOptimizer() {}

void SystemOptimizer::setup() {
  setObjective();
  cout << "Duplex initialization started. It make take a while to "
          "analyze the root."
       << endl;
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
}

void SystemOptimizer::initialize() {
  setup();
  double *init = getInitialState();
  double *reward = new double[parameterDimension];
  for (int i = 0; i < parameterDimension; i++)
    reward[i] = 1;
  // Setting up the root node
  State *root = new State(parameterDimension, objectiveDimension);
  root->setParameter(init);
  root->setReward(reward, (double)parameterDimension);
  root->setID(0);
  root->setParentID(-1);
  auto score = evaluate(root);
  stat->updateError(score);
  db->insert(root);
  cout << "Root node set." << endl;
}

void SystemOptimizer::optimize() {
  Duplex::optimize();
  if (settings->check("sensitivity-analysis.enable", "true"))
    stat->sensitivity->generateSensitivityMatrix();
}

void SystemOptimizer::post() {
  // overrides the post methods in nonconvex optimizer and does nothing
}

State *SystemOptimizer::localStep(int i, State *qnear) {
  computeTemperature(i);
  nextCandidateParameter = computeNextCandidateParameter(qnear);
  double *input = generateNewInput(qnear);
  State *qnew = new State(parameterDimension, objectiveDimension);
  qnew->setParameter(input);
  qnew->setParent(qnear);
  stat->sensitivity->pushBackInputChange(
      nextCandidateParameter, qnew->getParameter()[nextCandidateParameter],
      stepLength);
  return qnew;
}
