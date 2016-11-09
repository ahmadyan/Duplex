#include "nonconvexOptimizer.h"
#include <cmath>

NonconvexOptimizer::NonconvexOptimizer(Settings* s):Duplex(s){
    engine = "nonconvex-optimizer-duplex";
}

NonconvexOptimizer::~NonconvexOptimizer(){
}

double* NonconvexOptimizer::getInitialState(){
    double* init = new double[parameterDimension];
    bool initialStateAssignmentIsRandom = settings->check("initial_state_assignment", "random");
    
    vector<string> parameters = settings->listVariables("parameter", "uid-parameter");
    for(int i=0;i<parameters.size();i++){
        if (initialStateAssignmentIsRandom){
            double min = settings->lookupFloat(("parameter." + parameters[i] + ".range.min").c_str());
            double max = settings->lookupFloat(("parameter." + parameters[i] + ".range.max").c_str());
            init[i] = (max - min)*((1.0*rand()) / RAND_MAX) + min;
        }else{
            init[i] = settings->lookupFloat(("parameter." + parameters[i] + ".init").c_str());
        }
    }
    return init;
}

void NonconvexOptimizer::setObjective(){
    goal = new State(parameterDimension, objectiveDimension);
    vector<string> objectives = settings->listVariables("objective", "uid-objective");
    for (int i = 0; i < objectiveDimension; i++){
        string val = settings->lookupString(("objective." + objectives[i] + ".goal.optimum").c_str());
        goal->setObjective(stod(val), i);
    }
}

void NonconvexOptimizer::setup(){
    setObjective();
    cout << "Duplex initialization started. It make take a while to analyze the root." << endl;
    //setting boundaries for the objectives
    objectiveType = settings->listValues("objective", "uid-objective.goal.mode");
    vector<string> objectiveGoalMinStringVector = settings->listValues("objective", "uid-objective.goal.min");
    vector<string> objectiveGoalMaxStringVector = settings->listValues("objective", "uid-objective.goal.max");
    vector<string> objectiveMinStringVector = settings->listValues("objective", "uid-objective.min");
    vector<string> objectiveMaxStringVector = settings->listValues("objective", "uid-objective.max");
    vector<string> objectiveOptimumStringVector = settings->listValues("objective", "uid-objective.goal.optimum");
    goalRegionBoxMin = new double[objectiveDimension];
    goalRegionBoxMax = new double[objectiveDimension];
    max = new double[objectiveDimension];
    min = new double[objectiveDimension];
    opt = new double[objectiveDimension];
    for (int i = 0; i<objectiveDimension; i++){
        goalRegionBoxMin[i] = stod(objectiveGoalMinStringVector[i]);
        goalRegionBoxMax[i] = stod(objectiveGoalMaxStringVector[i]);
        min[i] = stod(objectiveMinStringVector[i]);
        max[i] = stod(objectiveMaxStringVector[i]);
        opt[i] = stod(objectiveOptimumStringVector[i]);
    }
    cout << "Goals are set." << endl;
    
    vector<string> parametersMinStringVector = settings->listValues("parameter", "uid-parameter.range.min");
    vector<string> parametersMaxStringVector = settings->listValues("parameter", "uid-parameter.range.max");
    parameterMin = new double[parameterDimension];
    parameterMax = new double[parameterDimension];
    for (int i = 0; i < parameterDimension; i++){
        parameterMin[i] = stod(parametersMinStringVector[i]);
        parameterMax[i] = stod(parametersMaxStringVector[i]);
    }
    stat = new Stat(settings, max, min, opt);
    cout << "Statistic class initialized." << endl;
}

void NonconvexOptimizer::initialize(){
    setup();
    double* init = getInitialState();
    double* reward = new double[parameterDimension];
    for (int i = 0; i < parameterDimension; i++) reward[i] = 1;
    //Setting up the root node
    State* root = new State(parameterDimension, objectiveDimension);
    root->setParameter(init);
    root->setReward(reward, (double)parameterDimension);
    root->setID(0);
    root->setParentID(-1);
    auto score = evaluate(root);
    stat->updateError(score);
    db->insert(root);
    cout << "Root node set." << endl;
}

// There are three approaches for taking the global step
// 1. only generate samples in the goal region. This is very depth oriented and forces duplex
// to directly converge toward the goal region.	however duplex can get stuck in local minimas
// along the  path, which forces us to make the goal region bigger, thus makes duplex behaves
// more like RRT in the extreme case where goal region is the entire state space, duplex is RRT.
// 2. Similar to (1), we start by covering a big region of the state space at the begining, but
// we shrink the goal region as the algorithm progresses. In duplex, the size of the goal region
// is directly related to the temperature. In the end, all our samples will be around the goal state.
//	3. fitness: this is usefull in cases where we don't have a clear definition of the goal region.
// In that case, instead of generating a goal sample and picking the closest node, we pick the
// best fitted node among the nodes in the random tree. In order to to that, we keep a priority-queue
// of the X numbers of best observed nodes so far and pick randomly among them. This is the same
// as survival of the fittest concept in genetic algorithms. However, toward the end of the algorithm,
// duplex becomes very depth-oriented.
State* NonconvexOptimizer::globalStep(){
    State* qsample = new State(parameterDimension, objectiveDimension);
    qsample->setParameter(qsample->uniformRandomVector(parameterDimension, goalRegionBoxMin, goalRegionBoxMax));
    if (shrinkGoalRegionWithTemperateOption){
        double* objective = new double[objectiveDimension];
        for (int i = 0; i < objectiveDimension; i++){
            delta = abs(goalRegionBoxMax[i] - goalRegionBoxMin[i]);
            objective[i] = qsample->unifRand(opt[i] - temperature*delta, opt[i] + temperature*delta);
        }
        qsample->setObjective(objective);
    }else{
        qsample->setObjective(qsample->uniformRandomVector(objectiveDimension, goalRegionBoxMin, goalRegionBoxMax));
    }
    return db->nearestNode(qsample);
}

int NonconvexOptimizer::computeNextCandidateParameter(State* qnear){
    nextCandidateParameter = rand() % qnear->getParameterSize();
    return nextCandidateParameter;
}

void NonconvexOptimizer::computeTemperature(int i){
    switch (temperatureOption){
        case Temperature::temperaturefast:
            temperature = t0 * (1- (1.0*i) / iterationCap );
            break;
        case Temperature::temperatureboltz:
            temperature = t0 / log(i+2);
            break;
        case Temperature::temperatureexp:
            temperature = temperature*0.95;
            break;
    }
    if (temperature < 0.05) temperature = 0.05;
}

double NonconvexOptimizer::computeStepLength(){
    double maxStep = initialStepLength * rand() / double(RAND_MAX);
    int direction = 1; if (rand() % 2 == 0) direction = -1;
    switch (annealingOption){
        case Annealing::annealingfast:
            stepLength = direction * initialStepLength * temperature;
            break;
        case Annealing::annealingboltz:
            stepLength = direction * initialStepLength * sqrt(temperature);
            break;
        case Annealing::annealingboltzrandom:
            stepLength = direction * maxStep * sqrt(temperature);
            break;
        case Annealing::annealingfastrandom:
            stepLength = direction * maxStep * temperature;
            break;
    }
    return stepLength;
}

double* NonconvexOptimizer::generateNewInput(State* q){
    int pSize = q->getParameterSize();
    double* param = q->getParameter();
    double* input = new double[pSize];
    for (int i = 0; i < pSize; i++){
        input[i] = param[i] + computeStepLength();
        if (input[i] < parameterMin[i]) input[i] = parameterMin[i];
        if (input[i] > parameterMax[i]) input[i] = parameterMax[i];
    }
    //input[nextCandidateParameter] += stepLength;
    //if (input[nextCandidateParameter] < parameterMin[nextCandidateParameter]) input[nextCandidateParameter] = parameterMin[nextCandidateParameter];
    //if (input[nextCandidateParameter] > parameterMax[nextCandidateParameter]) input[nextCandidateParameter] = parameterMax[nextCandidateParameter];
    return input;
}

State* NonconvexOptimizer::localStep(int i, State* qnear){
    computeTemperature(i);
    //auto nextCandidateParameter = computeNextCandidateParameter(qnear);
    double* input = generateNewInput(qnear);
    State* qnew = new State(parameterDimension, objectiveDimension);
    qnew->setParameter(input);
    qnew->setParent(qnear);
    return qnew;
}

double NonconvexOptimizer::evaluate(State* qnew){
    system->eval(qnew, 0);                  //simulate the circuit with the
    score(qnew, max, min);
    return qnew->getScore();
}

void NonconvexOptimizer::post() {
	cout << "Optimum solution:" << optimum->getScore() << endl;
	for (int i = 0; i<parameterDimension; i++) {
		cout << optimum->getParameter()[i] << " ";
	}
}

bool NonconvexOptimizer::isConverged(int iteration, State* q){
    return iteration<iterationCap;
}
