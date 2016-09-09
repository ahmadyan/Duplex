#include "descentOptimizer.h"
#include "gradientDescent.h"
#include "adagrad.h"
#include "adam.h"
#include "adadelta.h"
#include "nadam.h"
#include <cmath>

DescentOptimizer::DescentOptimizer(Settings* s):Duplex(s){
    if(settings->check("optimization.algorithm", "gd")){
        optimizer = new GradientDescent(settings);
    }else if(settings->check("optimization.algorithm", "adagrad")){
        optimizer  = new Adagrad(settings);
    }else if(settings->check("optimization.algorithm", "nadam")){
        optimizer  = new Nadam(settings);
    }else if(settings->check("optimization.algorithm", "adam") || settings->check("optimization.algorithm", "adamax")){
        optimizer = new Adam(settings);
    }else if(settings->check("optimization.algorithm", "adadelta")){
        optimizer  = new Adadelta(settings);
    }else{
        optimizer = new GradientDescent(settings);
    }
}

DescentOptimizer::~DescentOptimizer(){
}

double* DescentOptimizer::getInitialState(){
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

State* DescentOptimizer::initialize(){
    cout << "Duplex initialization started. It make take a while to analyze the root." << endl;
    double* init = getInitialState();
    max = new double[objectiveDimension];
    min = new double[objectiveDimension];
    vector<string> objectiveMinStringVector = settings->listValues("objective", "uid-objective.min");
    vector<string> objectiveMaxStringVector = settings->listValues("objective", "uid-objective.max");
    for (int i = 0; i<objectiveDimension; i++){
        min[i] = stod(objectiveMinStringVector[i]);
        max[i] = stod(objectiveMaxStringVector[i]);
    }
    stat = new Stat(settings, max, min, opt);
    //Setting up the root node
    State* root = new State(parameterDimension, objectiveDimension);
    root->setParameter(init);
    root->setID(0);
    root->setParentID(-1);
    system->eval(root);
    return root;
}

State* DescentOptimizer::globalStep(){
    return db->getState();
}

State* DescentOptimizer::localStep(int i, State* qnear){
    return optimizer->update(qnear);
}

double DescentOptimizer::evaluate(State* qnew){
    system->eval(qnew, 0);
    return qnew->getObjective()[0];
}

bool DescentOptimizer::isConverged(int iteration, State* q){
    stat->updateConvergence(q);
    auto epsilon = settings->lookupFloat("optimization.epsilon");
    return ( (stat->getDeltaConvergence()>epsilon) && iteration<iterationCap );
}
