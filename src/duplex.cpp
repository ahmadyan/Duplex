//
//  duplex.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//

#include "duplex.h"
#include "system.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <fstream>

#include "optimizer.h"
#include "gradientDescent.h"
#include "adagrad.h"
#include "adam.h"
#include "adadelta.h"
#include "nadam.h"

using namespace std;

Duplex::Duplex(Settings* c){
	settings = c;
	iterationCap = settings->lookupInt("iterations");
	parameterDimension = settings->lookupInt("parameter.size");
	objectiveDimension = settings->lookupInt("objective.size");

	t0 = settings->lookupFloat("initial_temperature");
	temperature = t0;
	initialStepLength = settings->lookupFloat("initial_step_length");
	reinforcementLearningOption = settings->check("reinforcement_learning", "enable");
	shrinkGoalRegionWithTemperateOption = settings->check("shrink_goal_region_with_temperate", "enable");
	if (reinforcementLearningOption){
		minAward = settings->lookupFloat("min_reward");
		maxAward = settings->lookupFloat("max_reward");
		delta = settings->lookupFloat("delta_reward");
	}
	if (settings->check("temperature", "fast")){
		temperatureOption = Temperature::temperaturefast;
	}else if (settings->check("temperature", "boltz")){
		temperatureOption = Temperature::temperatureboltz;
	}else if (settings->check("temperature", "exp")){
		temperatureOption = Temperature::temperatureexp;
	}else{
		cout << "Temperature option not found in the config file. Possible options are [fast, boltz, exp].";
		exit(2);
	}

	if (settings->check("annealing", "fast")){
		annealingOption = Annealing::annealingfast;
	}else if (settings->check("annealing", "boltz")){
		annealingOption = Annealing::annealingboltz;
	}else if (settings->check("annealing", "fast-random")){
		annealingOption = Annealing::annealingfastrandom;
	}else if (settings->check("annealing", "boltz-random")){
		annealingOption = Annealing::annealingboltzrandom;
	}else{
		cout << "Annealing option not found in the config file/ Possible options are [fast, boltz]";
		exit(2);
	}
    
    db = new Search(settings, objectiveDimension);
}

Duplex::~Duplex(){
	clear();
}

double* Duplex::getInitialState(){
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

void Duplex::initialize(){
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
		parameterMin[i] = stod(parametersMinStringVector[i]);;
		parameterMax[i] = stod(parametersMaxStringVector[i]);;
	}

    stat = new Stat(settings, max, min, opt);
    cout << "Statistic class initialized." << endl;
    
	//Setting up the root node
	root = new State(parameterDimension, objectiveDimension);
	root->setParameter(init);
	root->setReward(reward, (double)parameterDimension);
	root->setID(0);
	root->setParentID(-1);
	system->eval(root);
	double distance = score(root, max, min);
    stat->updateError(distance);
	db->insert(root);
	cout << "Root node set." << endl;

	if (settings->check("mode", "fopt")){
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

	cout << "Error and distance set." << endl;
}

void Duplex::setObjective(){
	goal = new State(parameterDimension, objectiveDimension);
	vector<string> objectives = settings->listVariables("objective", "uid-objective");
	for (int i = 0; i < objectiveDimension; i++){
		string val = settings->lookupString(("objective." + objectives[i] + ".goal.optimum").c_str());
		goal->setObjective(stod(val), i);
	}
}

void Duplex::setSystem(System* sys){
    system=sys;
}

// There are three approaches for taking the global step
// 1. only generate samples in the goal region. This is very depth oriented and forces duplex to directly converge toward the goal region.
//		however duplex can get stuck in local minimas along the  path, which forces us to make the goal region bigger --> makes duplex behaves more like RRT
//		in the extreme case where goal region is the entire state space, duplex is RRT.
//	2. Similar to (1), we start by covering a big region of the state space at the begining, but we shrink the goal region as the algorithm progresses. In duplex, the size of
//		the goal region is directly related to the temperature. In the end, all our samples will be around the goal state.
//	3. fitness: this is usefull in cases where we don't have a clear definition of the goal region. In that case, instead of generating a goal sample and picking 
//		the closest node, we pick the best fitted node among the nodes in the random tree. In order to to that, we keep a priority-queue of the X numbers of best observed
//		nodes so far and pick randomly among them. This is the same as survival of the fittest concept in genetic algorithms. However, toward the end of the algorithm, duplex
//		becomes very depth-oriented.
State* Duplex::globalStep(){
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
    return qsample;
}


// Evaluates each states and assign them a score.
// States with lower score are better candidates, state with the minimum score (0) is the optimal solution.
double Duplex::score(State* state, double* maxBound, double* minBound){
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

//slightly changes the input
double* Duplex::generateNewInput(State* q){
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

int Duplex::computeNextCandidateParameter(State* qnear){
	if (reinforcementLearningOption){
		double random = qnear->getRewardCDF() * ((double)rand() / (RAND_MAX));
		double* reward = qnear->getRewardVector();
		for (int i = 0; i < parameterDimension; i++){
			random -= reward[i];
			if (random <= 0) return i;
		}
		return parameterDimension - 1;
	}else{
		nextCandidateParameter = rand() % qnear->getParameterSize();
		return nextCandidateParameter;
	}
}

void Duplex::computeTemperature(int i){
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

double Duplex::computeStepLength(){ 
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

State* Duplex::localStep(int i, State* qnear){
	computeTemperature(i);
	nextCandidateParameter = computeNextCandidateParameter(qnear);
	double* input = generateNewInput(qnear);
	State* qnew = new State(parameterDimension, objectiveDimension);
	qnew->setParameter(input);
	qnew->setParent(qnear);
	stat->sensitivity->pushBackInputChange(nextCandidateParameter, qnew->getParameter()[nextCandidateParameter], stepLength);
	return qnew;
}



//Optimizer types:
// Duplex internally supports three types of optimizer
//  1. Tree optimizer, which is based on random tree optimization algorithm
//  2. Walk optimizer, a generic term for descent algorithms, such as gradient descent, Adam, RMSProp, etc.
//  3. Heuristic methods, currently supporting only simulated annealing

//todo: needs refactoring, should be moved to its own class
// Generic walk-based optimizer methods, such as gradient descent, Adap, RMPProp, etc.
void Duplex::walkOptimizer(){
    auto epsilon = settings->lookupFloat("optimization.epsilon");
    Optimizer* optimizer;
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
    
    auto iterations=1;
    while(stat->getDeltaConvergence()>epsilon && (iterations<iterationCap)){
        State* prev = db->getState();
        State* next = optimizer->update(prev);
        system->eval(next,0);
        insert(iterations, prev, next);
        stat->updateConvergence(next);
        iterations++;
    }
    cout << "Optimizer finished, #iteration= " << iterations << endl ;

}

void Duplex::treeOptimizer(){
    for (int i = 1; i<iterationCap; i++){
        cout << i << endl;
        State* qsample = globalStep();              //generate a new bias sample
        State* qnear = db->nearestNode(qsample);        //Find closest node to the objective
        State* qnew = localStep(i, qnear);
        system->eval(qnew, 0);                  //simulate the circuit with the new input
        insert(i, qnear, qnew);
    }
    
    if (settings->check("sensitivity-analysis.enable", "true"))
        stat->sensitivity->generateSensitivityMatrix();
}


void Duplex::optimize(){
    //treeOptimizer();
    walkOptimizer();
}

void Duplex::simulated_annealing(){
	State* bestState = root;
	auto bestEnergy = goal->distance(root, max, min);
	for (int i = 1; i<iterationCap; i++){
		cout << i << endl;
		auto qsample = new State(parameterDimension, objectiveDimension);
		qsample->setParameter(qsample->uniformRandomVector(parameterDimension, goalRegionBoxMin, goalRegionBoxMax));
		system->eval(qsample, 0);  
		auto r = (1.0*rand()) / RAND_MAX;
		auto energy = goal->distance(qsample, max, min);
		auto t = exp((bestEnergy - energy) / (20 * i));
		cout << "random=" << r << " temperature=" << t << " energy=" << energy << " best" << bestEnergy << endl;
		if ((energy < bestEnergy) || (t<r)){
			bestState = qsample;
			bestEnergy = energy;
		}
		stat->updateError(score(bestState, max, min));
	}
}

//skratchpad, should be merged with globalStep function
State* Duplex::foptGlobalStep(){
	State* qsample = new State(parameterDimension, objectiveDimension);
	qsample->setParameter(qsample->uniformRandomVector(parameterDimension, goalRegionBoxMin, goalRegionBoxMax));
	qsample->setObjective(qsample->uniformRandomVector(objectiveDimension, goalRegionBoxMin, goalRegionBoxMax));
	//for (int i = 0; i < objectiveDimension; i++){
	//	cout << qsample->getObjective()[i] << " ,";
	//}cout << endl;
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

void Duplex::insert(int i, State* qnear, State* qnew){
    qnew->setID(i);
    qnew->setParent(qnear);
    db->insert(qnew);                       //add a new node to the database
}

/// functional optimization algorithm using Duplex
/// the big difference between fopt and duplex is that in fopt, we are optimizing for an entire path, not just for one state
void Duplex::functionalOptimization(){
	for (int i = db->getSize(); i < iterationCap; i++){
		cout << i << endl;
		State* qsample = foptGlobalStep();              //generate a new bias sample
		State* qnear = db->nearestNode(qsample);        //Find closest node to the objective
		State* qnew = localStep(i, qnear);
		system->eval(qnew, 0);                  //simulate the circuit with the new input
		//bias.push_back(qsample);
		//updateError(score(qnew, max, min));
		//updateReward(qnear, qnew);
		//updateSensitivity(qnear, qnew);
        insert(i, qnew, qnear);
	}
}

void Duplex::kmean(){
    cout << "Executing standard kmean clustering algorithm" << endl ;
    // need two database: 1 for holding u1...uk, other one for the actual data samples
    // need plotting mechanism for scatter plot of data
    auto m = settings->lookupInt("data.size");
    int k = settings->lookupInt("clustering.clusters");
    int d = settings->lookupInt("data.dimension");
    
    samples = new Data(settings);
    
    //parameter size = k*d
    //objective size = uknown
    
    //generate data
    //initialize k cluster
    //while(conv)
    //  for i=1:m --> assign nodes to clusters
    //  for i=1:k --> update clusters
    
    
    samples->exportData("tmp");
    
}

void Duplex::clear(){
}

double minimum(double a, double b, double min){
	double m = min;
	if (a<m) m = a;
	if (b<m) m = b;
	return m;
}

double maximum(double a, double b, double max){
	double m = max;
	if (a>m) m = a;
	if (b>m) m = b;
	return m;
}

string Duplex::drawCanvas(string function, double xmin, double xmax, double ymin, double ymax, string plotType, string title){
    stringstream canvas;
    if(plotType=="default"){
        canvas << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0 title '" << title << "' with linespoints lt \"white\" pt 0.01\n";
    }else{
        canvas
        << "reset\n"
        
        << "f(x,y)="<< function << "\n"
        << "set xrange [" << xmin << ":" << xmax <<"]\n"
        << "set yrange [" << ymin << ":" << ymax <<"]\n"
        << "set isosample 250, 250\n"
        << "set table 'test.dat'\n"
        << "splot f(x,y)\n"
        << "unset table\n"
        
        << "set contour base\n"
        << "set cntrparam level incremental -3, 0.5, 3\n"
        << "unset surface\n"
        << "set table 'cont.dat'\n"
        << "splot f(x,y)\n"
        << "unset table\n"
        
        << "reset\n"
        << "set xrange [" << xmin << ":" << xmax <<"]\n"
        << "set yrange [" << ymin << ":" << ymax <<"]\n"
        << "unset key\n"
        << "set palette rgbformulae 33,13,10\n";
        
        if(plotType=="contour"){
            canvas << "p 'test.dat' with image, 'cont.dat' with linespoints lt \"white\" pt 0.01\n";
        } //the actual default is the surf plot
    }
    return canvas.str();
}

//	Plotting methods for Duplex
//  todo: needs refactoring, needs to go into their own class
string Duplex::drawParameterTree(int x, int y, int z, string sizingPreference, string plotType, string title){
	string color = "blue";
	stringstream cmdstr;
	double xmin = 99, xmax = -99, ymin = 99, ymax = -99;
	for (int i = 1; i<db->getSize(); i++){   // i starts at 1 because we ignore the root
		State* s = db->getState(i);
		State* p = db->getState(s->getParentID());
		double iFromX = p->getParameter()[x];
		double iFromY = p->getParameter()[y];
		double iToX = s->getParameter()[x];
		double iToY = s->getParameter()[y];
        auto iFromZ = p->getObjective()[0];
        auto iToZ = s->getObjective()[0];

		xmin = minimum(iFromX, iToX, xmin);
		ymin = minimum(iFromY, iToY, ymin);
		xmax = maximum(iFromX, iToX, xmax);
		ymax = maximum(iFromY, iToY, ymax);

        cmdstr << " set arrow from " << iFromX << "," << iFromY << "," << iFromZ << " to " << iToX << "," << iToY << "," << iToZ << " nohead  lc rgb \"" << color << "\" lw 2 front \n";
	}
    
    if(sizingPreference=="default"){
        xmin=-1;xmax=1;ymin=-1;ymax=1;
    }
    
	stringstream board;
    board << drawCanvas(system->getFunction(0), xmin, xmax, ymin, ymax, plotType, title);
    board << cmdstr.str();
    return board.str();
}

string Duplex::drawObjectiveTree(int x, int y, string title){
	string color = "red";
	stringstream cmdstr;
	
	double xmin = 99, xmax = -99, ymin = 99, ymax = -99;
	for (int i = 1; i<db->getSize(); i++){   // i starts at 1 because we ignore the root
		State* s = db->getState(i);
		State* p = db->getState(s->getParentID());
		double iFromX = p->getObjective()[x];
		double iFromY = p->getObjective()[y];
		double iToX = s->getObjective()[x];
		double iToY = s->getObjective()[y];
		xmin = minimum(iFromX, iToX, xmin);
		ymin = minimum(iFromY, iToY, ymin);
		xmax = maximum(iFromX, iToX, xmax);
		ymax = maximum(iFromY, iToY, ymax);
		cmdstr << " set arrow from " << iFromX << "," << iFromY << " to " << iToX << "," << iToY << " nohead  lc rgb \"" << color << "\" lw 2 \n";
	}
    
	stringstream board;
	board << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0  title '" << title << "' with linespoints lt \"white\" pt 0.01";
	cmdstr << board.str() << "\n " << cmdstr.str();
	return cmdstr.str();
}

string Duplex::drawTrace(int x, int y, string title){
	string color = "black";
	stringstream cmdstr;

	double xmin = 99, xmax = -99, ymin = 99, ymax = -99;
	State* s = db->getOptimum();
    cout << "optimum state = " << s->getID() << ", " << s->getObjective()[0] << " , " << s->getObjective()[1] << ", " << s->getObjective()[2] << endl ;

    //todo: clean this up
    int maxarea=-1;
    int index=-1;
    for(int i=0;i<db->getSize();i++){
        State* x = db->getState(i);
        if(x->getObjective()[0]<50 && x->getObjective()[1]<100){
            double z = x->getObjective()[2];
            if(z>maxarea){
                z=maxarea;
                index=i;
            }
        }
    }
    s = db->getState(index);
    cmdstr << " set arrow from " << 50 << "," << 0 << " to " << s->getParameter()[x] << "," << s->getParameter()[y] << " nohead  lc rgb \"" << color << "\" lw 2 \n";
    while (s->getID() != 0){
		State* p = db->getState(s->getParentID());
		double iFromX = p->getParameter()[x];
		double iFromY = p->getParameter()[y];
		double iToX = s->getParameter()[x];
		double iToY = s->getParameter()[y];
		xmin = minimum(iFromX, iToX, xmin);
		ymin = minimum(iFromY, iToY, ymin);
		xmax = maximum(iFromX, iToX, xmax);
        if(xmax<50) xmax=52;    //todo: needs cleanup
		ymax = maximum(iFromY, iToY, ymax);
		cmdstr << " set arrow from " << iFromX << "," << iFromY << " to " << iToX << "," << iToY << " nohead  lc rgb \"" << color << "\" lw 2 \n";
		s = p;
	}

	stringstream board;
	board << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0  title '" << title << "' with linespoints lt \"white\" pt 0.01";
	cmdstr << board.str() << "\n " << cmdstr.str();
	return cmdstr.str();
}

string Duplex::draw(int i){
	vector<string> plots = settings->listVariables("plot", "uid-plot");
	string type = settings->lookupString(("plot." + plots[i] + ".type").c_str());
	if (type == "error"){
		return stat->plotError();
    }else if (type == "data"){
        return samples->plotScatter();
    }
	else if (type == "distance"){
		return stat->plotDistance();
	}
	else if (type == "tree.parameter"){
		auto x = settings->lookupInt(("plot." + plots[i] + ".x").c_str());
		auto y = settings->lookupInt(("plot." + plots[i] + ".y").c_str());
        auto z = settings->lookupInt(("plot." + plots[i] + ".z").c_str());
        auto plotPreference = settings->lookupString(("plot." + plots[i] + ".plot").c_str());
        auto sizingPreference = settings->lookupString(("plot." + plots[i] + ".size").c_str());
        if((x>parameterDimension-1) || (y>parameterDimension-1) || (z>objectiveDimension-1)){
            cout << "error: tree.parameter plotting error, configuration file contains an invalid parameter x,y,z " << endl ;
            return "";
        }else{
            return drawParameterTree(x, y, z, sizingPreference, plotPreference, "param");
        }
	}
	else if (type == "tree.objective"){
		int x = settings->lookupInt(("plot." + plots[i] + ".x").c_str());
		int y = settings->lookupInt(("plot." + plots[i] + ".y").c_str());
		return drawObjectiveTree(x, y, "objective");
	}
	else if (type == "trace"){
		int x = settings->lookupInt(("plot." + plots[i] + ".x").c_str());
		int y = settings->lookupInt(("plot." + plots[i] + ".y").c_str());
		return drawTrace(x, y, "trace");
	}else{
        return "";
    }
}

void Duplex::save(boost::property_tree::ptree* ptree){
    ptree->add("duplex.version", 1);
    boost::property_tree::ptree& data = ptree->add("duplex.data", "");
    db->save(&data);
	boost::property_tree::ptree& sense = ptree->add("duplex.sensitivity", "");
	stat->sensitivity->save(&sense);
    boost::property_tree::ptree& node = ptree->add("duplex.stat", "");
	
	for (int i = 0; i<db->getSize(); i++){
        boost::property_tree::ptree& iter = node.add("iteration", "");
        iter.add("id", i);
        iter.add("error", stat->getError(i));
        iter.add("distance", stat->getDistance(i));
        iter.put("<xmlattr>.id", i);
    }
}

void Duplex::load(boost::property_tree::ptree* ptree){
    boost::property_tree::ptree& data = ptree->get_child("duplex.data");
    db->load(&data);
    auto pnodes = ptree->get_child("duplex.stat");
    for(auto v: pnodes){
        if(v.first=="iteration"){
            cout << v.second.get<int>("id") << endl;
            stat->error_push_back(v.second.get<double>("error"));
            stat->distance_push_back(v.second.get<double>("distance"));
        }
    }
}
