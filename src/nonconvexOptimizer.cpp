#include "nonconvexOptimizer.h"
#include <cmath>

NonconvexOptimizer::NonconvexOptimizer(Settings* s):Duplex(s){
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

State* NonconvexOptimizer::initialize(){
    setObjective();
    cout << "Duplex initialization started. It make take a while to analyze the root." << endl;
    double* init = getInitialState();
    double* reward = new double[parameterDimension];
    
    for (int i = 0; i < parameterDimension; i++)
        reward[i] = 1;
    
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
    
    //Setting up the root node
    State* root = new State(parameterDimension, objectiveDimension);
    root->setParameter(init);
    root->setReward(reward, (double)parameterDimension);
    root->setID(0);
    root->setParentID(-1);
    system->eval(root);
    double distance = score(root, max, min);
    stat->updateError(distance);
    //db->insert(root);
    cout << "Root node set." << endl;
    return root;
}

void NonconvexOptimizer::insert(State* s){
    db->insert(s);
}

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

// Evaluates each states and assign them a score.
// States with lower score are better candidates, state with the minimum score (0) is the optimal solution.
double NonconvexOptimizer::score(State* state, double* maxBound, double* minBound){
    double distance = 0;
    // In functional optimization, we have to evaluat each objective seperately.
    if (settings->check("mode", "fopt")){
        double b = settings->lookupFloat("parameter.b");
        double c0 = settings->lookupFloat("parameter.c0");
        double* boundary = new double[2];
        boundary[0] = b;
        boundary[1] = 0;
        
        double* objectives = state->getObjective();
        double* goals = goal->getObjective();
        int objectiveSize = state->getObjectiveSize();
        //measure objective
        //currently we support the following types of objectives
        //1. boundary: equivalent to the boundary conditions in BVPs. The closer we get to the boundary value, the better
        //2. boundary-strict: similar to boundary, but if we are crossing the bounary value the sameples is not usefull anymore.
        //   for example, for the length of the curve we use bounadry-hard objectives, so the length is strictly less than boundary
        //3. maximize
        //4. minimize
        for (int i = 0; i < objectiveSize; i++){
            auto potentialdistance = state->distance(2, state->getParameter(), boundary);
            auto potentialsum = (state->getParameter()[1] * (b-state->getParameter()[0]) ) / 2.0 ;
            double normalizedDistance = (objectives[i] - goals[i]) / (max[i] - min[i]);
            normalizedDistance *= normalizedDistance;
            if ((objectiveType[i] == "boundary-strict")
                && (objectives[i]>opt[i])                   //if the length of the curve is bigger than the strict boundary, discard this trace
                && (objectives[i]+potentialdistance>opt[i]) //evaluating the potential of this sample: if we even take the direct route to the boundary
                //condition and still the length is more than the bondary, discard this sample
                ){
                normalizedDistance = std::numeric_limits<int>::max();
            }
            if ((objectiveType[i] == "maximize")
                && (objectives[i]>max[i])
                && (objectives[i] + potentialsum > max[i])){
                normalizedDistance = 0;
            }
            distance += normalizedDistance;
        }
        delete[] boundary;    //todo: remove this
    }else{
        //Most of the time, we can use Eucledean distance as the score. Closer to the goal, the better
        distance = goal->distance(state, maxBound, minBound);
    }
    
    state->setScore(distance);
    return distance;
}

bool NonconvexOptimizer::isConverged(int iteration){
    return iteration<iterationCap;
}
