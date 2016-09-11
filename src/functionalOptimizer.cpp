#include "functionalOptimizer.h"
#include <cmath>

FunctionalOptimizer::FunctionalOptimizer(Settings* s):Duplex(s){
}

FunctionalOptimizer::~FunctionalOptimizer(){
}

double* FunctionalOptimizer::getInitialState(){
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

void FunctionalOptimizer::setObjective(){
    goal = new State(parameterDimension, objectiveDimension);
    vector<string> objectives = settings->listVariables("objective", "uid-objective");
    for (int i = 0; i < objectiveDimension; i++){
        string val = settings->lookupString(("objective." + objectives[i] + ".goal.optimum").c_str());
        goal->setObjective(stod(val), i);
    }
}

void FunctionalOptimizer::initialize(){
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
    insert(0, root, root);
    cout << "Root node set." << endl;
    
    //generates an initial curve, starting at y(0)=0 and ending in y(n)=b
    double b = settings->lookupFloat("parameter.b");
    double c0 = settings->lookupFloat("parameter.c0");
    int pathSegments = settings->lookupInt("initialPathSegments");
    
    //randomly generate each segment in the path
    for (int i = 1; i < pathSegments-1; i++){
        State* q = new State(parameterDimension, objectiveDimension);
        double* p = new double[parameterDimension];
        //p[0] = i;	//time, ignore this, the states are no longer time-annotated in dido case
        for (int j = 0; j < parameterDimension; j++){
            p[j] = q->unifRand(parameterMin[j], parameterMax[j]);
        }
        q->setParameter(p);
        q->setID(i);
        q->setParent(db->getState(i - 1));
        system->eval(q);
        distance = score(q, max, min);
        stat->updateError(distance);
        db->insert(q);
    }
    
    //connect the last point to b
    State* last = new State(parameterDimension, objectiveDimension);
    double* p = new double[parameterDimension];
    //p[0] = pathSegments;
    p[0] = b;
    p[1] = 0;
    last->setParameter(p);
    last->setID(pathSegments - 1);
    last->setParent(db->getState(pathSegments - 2));
    system->eval(last);
    distance = score(last, max, min);
    stat->updateError(distance);
    db->insert(last);
    cout << "Initial curve is set." << endl;
}

State* FunctionalOptimizer::globalStep(){
    State* qsample = new State(parameterDimension, objectiveDimension);
    qsample->setParameter(qsample->uniformRandomVector(parameterDimension, goalRegionBoxMin, goalRegionBoxMax));
    qsample->setObjective(qsample->uniformRandomVector(objectiveDimension, goalRegionBoxMin, goalRegionBoxMax));
    if (shrinkGoalRegionWithTemperateOption){
        double* objective = new double[objectiveDimension];
        for (int i = 0; i < objectiveDimension; i++){
            delta = abs(goalRegionBoxMax[i] - goalRegionBoxMin[i]);
            objective[i] = qsample->unifRand(opt[i] - temperature*delta, opt[i] + temperature*delta);
        }
        qsample->setObjective(objective);
    }
    else{
        qsample->setObjective(qsample->uniformRandomVector(objectiveDimension, goalRegionBoxMin, goalRegionBoxMax));
    }
    return qsample;
}

int FunctionalOptimizer::computeNextCandidateParameter(State* qnear){
    nextCandidateParameter = rand() % qnear->getParameterSize();
    return nextCandidateParameter;
}

void FunctionalOptimizer::computeTemperature(int i){
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

double FunctionalOptimizer::computeStepLength(){
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

double* FunctionalOptimizer::generateNewInput(State* q){
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

State* FunctionalOptimizer::localStep(int i, State* qnear){
    computeTemperature(i);
    //auto nextCandidateParameter = computeNextCandidateParameter(qnear);
    double* input = generateNewInput(qnear);
    State* qnew = new State(parameterDimension, objectiveDimension);
    qnew->setParameter(input);
    qnew->setParent(qnear);
    return qnew;
}

double FunctionalOptimizer::evaluate(State* qnew){
    system->eval(qnew, 0);                  //simulate the circuit with the
    score(qnew, max, min);
    return qnew->getScore();
}

bool FunctionalOptimizer::isConverged(int iteration, State* q){
    return iteration<iterationCap;
}
