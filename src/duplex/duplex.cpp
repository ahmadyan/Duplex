#include "duplex.h"

Duplex::Duplex(Settings *c) {
  settings = c;
  iterationCap = settings->lookupInt("iterations");
  system = new System(settings);

  parameterDimension = settings->lookupInt("parameter.size");
  objectiveDimension = settings->lookupInt("objective.size");

  t0 = settings->lookupFloat("initial_temperature");
  temperature = t0;
  initialStepLength = settings->lookupFloat("initial_step_length");
  reinforcementLearningOption =
      settings->check("reinforcement_learning", "enable");
  shrinkGoalRegionWithTemperateOption =
      settings->check("shrink_goal_region_with_temperate", "enable");
  if (reinforcementLearningOption) {
    minAward = settings->lookupFloat("min_reward");
    maxAward = settings->lookupFloat("max_reward");
    delta = settings->lookupFloat("delta_reward");
  }
  if (settings->check("temperature", "fast")) {
    temperatureOption = Temperature::temperaturefast;
  } else if (settings->check("temperature", "boltz")) {
    temperatureOption = Temperature::temperatureboltz;
  } else if (settings->check("temperature", "exp")) {
    temperatureOption = Temperature::temperatureexp;
  } else {
    cout << "Temperature option not found in the config file. Possible options "
            "are [fast, boltz, exp].";
    exit(2);
  }

  if (settings->check("annealing", "fast")) {
    annealingOption = Annealing::annealingfast;
  } else if (settings->check("annealing", "boltz")) {
    annealingOption = Annealing::annealingboltz;
  } else if (settings->check("annealing", "fast-random")) {
    annealingOption = Annealing::annealingfastrandom;
  } else if (settings->check("annealing", "boltz-random")) {
    annealingOption = Annealing::annealingboltzrandom;
  } else {
    cout << "Annealing option not found in the config file/ Possible options "
            "are [fast, boltz]";
    exit(2);
  }

  db = new Search(settings, objectiveDimension);
}

Duplex::~Duplex() {}

void Duplex::insert(int i, State *qnear, State *qnew) {
  qnew->setID(i);
  qnew->setParent(qnear);
  db->insert(qnew);
}

void Duplex::optimize() {
  cout << "Executing duplex optimization ... engine = " << engine << endl;
  initialize();
  int iteration = (int)(db->getSize());
  State *qnew;
  do {
    auto qprev = globalStep();
    qnew = localStep(iteration, qprev);
    auto cost = evaluate(qnew);
    insert(iteration, qprev, qnew);
  } while (isConverged(iteration++, qnew));
  post();
}

void Duplex::train() {
  cout << "Training the model ..." << endl;
  optimize();
}

Search *Duplex::getDB() { return db; }

System *Duplex::getSystem() { return system; }

void Duplex::setStat(Stat *s) { stat = s; }

Stat *Duplex::getStat() { return stat; }

// Evaluates each states and assign them a score.
// States with lower score are better candidates, state with the minimum score
// (0) is the optimal solution.
double Duplex::score(State *state, double *maxBound, double *minBound) {
  // Most of the time, we can use Eucledean distance as the score. Closer to the
  // goal, the better if a particular model needs a better scoring method, they
  // can over-ride this function (similar to functional optimizer class)
  auto distance = goal->distance(state, maxBound, minBound);
  state->setScore(distance);
  return distance;
}

void Duplex::post() { cout << "Duplex terminated successfully." << endl; }

void Duplex::save(boost::property_tree::ptree *ptree) {
  ptree->add("duplex.version", 1);
  boost::property_tree::ptree &data = ptree->add("duplex.data", "");
  db->save(&data);
  boost::property_tree::ptree &sense = ptree->add("duplex.sensitivity", "");
  stat->sensitivity->save(&sense);
  boost::property_tree::ptree &node = ptree->add("duplex.stat", "");

  for (int i = 0; i < db->getSize(); i++) {
    boost::property_tree::ptree &iter = node.add("iteration", "");
    iter.add("id", i);
    iter.add("error", stat->getError(i));
    iter.add("distance", stat->getDistance(i));
    iter.put("<xmlattr>.id", i);
  }
}

void Duplex::load(boost::property_tree::ptree *ptree) {
  boost::property_tree::ptree &data = ptree->get_child("duplex.data");
  db->load(&data);
  auto pnodes = ptree->get_child("duplex.stat");
  for (auto v : pnodes) {
    if (v.first == "iteration") {
      cout << v.second.get<int>("id") << endl;
      stat->error_push_back(v.second.get<double>("error"));
      stat->distance_push_back(v.second.get<double>("distance"));
    }
  }
}
