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
#include <sstream>
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
    
    db = new Search(settings);
	sensitivity = new Sensitivity(parameterDimension, objectiveDimension, (double)settings->lookupFloat("sensitivity-analysis.relativeSensitivityThreshold"));
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

	//Setting up the root node
	root = new State(parameterDimension, objectiveDimension);
	root->setParameter(init);
	root->setReward(reward, (double)parameterDimension);
	root->setID(0);
	root->setParentID(-1);
	system->eval(root);
	db->insert(root);
	cout << "Root node set." << endl;
	error.push_back(goal->distance(root, max, min));
	currentDistance.push_back(goal->distance(root, max, min));

	if (settings->check("mode", "fopt")){
		double b = settings->lookupFloat("parameter.b");
		double c0 = settings->lookupFloat("parameter.c0");
		int pathSegments = settings->lookupInt("initialPathSegments");

		for (int i = 1; i < pathSegments-1; i++){
			State* q = new State(parameterDimension, objectiveDimension);
			double* p = new double[parameterDimension];
			//p[0] = i;	//time
			for (int j = 0; j < parameterDimension; j++){
				p[j] = q->unifRand(parameterMin[j], parameterMax[j]);
			}
			q->setParameter(p);
			q->setID(i);
			q->setParent(db->getState(i - 1));
			system->eval(q);
			db->insert(q);
			error.push_back(0);
			currentDistance.push_back(0);
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
		db->insert(last);
		error.push_back(0);
		currentDistance.push_back(0);

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

void Duplex::updateReward(State* qnear, State* qnew){
	double distance = goal->distance(qnew, max, min);
	double pdistance = goal->distance(qnear, max, min);
	double* reward = new double[parameterDimension];
	double* preward = qnear->getRewardVector();
	for (int i = 0; i < parameterDimension; i++){
		reward[i] = preward[i];
	}

	double award = (distance - pdistance) / delta;
	if (award < minAward) award = minAward;
	if (award > maxAward) award = maxAward;
	reward[nextCandidateParameter] = +1;
	if (reward[nextCandidateParameter] < minAward) reward[nextCandidateParameter] = minAward;
	if (reward[nextCandidateParameter] > maxAward) reward[nextCandidateParameter] = maxAward;

	double sum = reward[nextCandidateParameter] - preward[nextCandidateParameter];

	qnew->setReward(reward, qnear->getRewardCDF() + sum); 
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
	sensitivity->pushBackInputChange(nextCandidateParameter, qnew->getParameter()[nextCandidateParameter], stepLength);
	return qnew;
}


void Duplex::optimize(){
    for(int i=1;i<iterationCap;i++){
		cout << i << endl;
		State* qsample = globalStep();              //generate a new bias sample
        State* qnear = db->nearestNode(qsample);        //Find closest node to the objective
		State* qnew = localStep(i, qnear); 
		system->eval(qnew, 0);                  //simulate the circuit with the new input
		update(i, qsample, qnear, qnew);
    }

	if (settings->check("sensitivity-analysis.enable", "true"))
		sensitivity->generateSensitivityMatrix();
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
		updateError(bestState, max, min);
	}
}

/// functional optimization algorithm using Duplex
/// the big difference between fopt and duplex is that in fopt, we are optimizing for an entire path, not just for one state
void Duplex::functionalOptimization(){
	
}


void Duplex::updateError(State* s, double* maxBound, double* minBound){
	double d = goal->distance(s, maxBound, minBound);
	double e = error[error.size() - 1];
	error.push_back(fmin(e, d));
    currentDistance.push_back(d);
}

void Duplex::updateSensitivity(State* qnear, State* qnew){
	double* obj = qnew->getObjective();
	double* obj_near = qnear->getObjective();
	for (int i = 0; i < objectiveDimension; i++){
		double delta = abs(obj[i] - obj_near[i]);
			sensitivity->pushBackOutputChange(i, obj[i], delta);
	}
	sensitivity->commit();
}

void Duplex::update(int i, State* qsample, State* qnear, State* qnew){
	db->insert(qnew);                       //add a new node to the database
	qnew->setID(i);
	qnew->setParentID(qnear->getID());      //maintaing the tree data structure
	bias.push_back(qsample);
	updateError(qnew, max, min);
	updateReward(qnear, qnew);
	updateSensitivity(qnear, qnew);
}

void Duplex::clear(){
	for (int i = 0; i<bias.size(); i++){
		delete bias[i];
	}
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

//
//		Plotting methods for Duplex
//
string Duplex::drawParameterTree(){
	string color = "blue";
	stringstream cmdstr;
	int x = settings->lookupInt("plot.x");
	int y = settings->lookupInt("plot.y");
	double xmin = 99, xmax = -99, ymin = 99, ymax = -99;
	for (int i = 1; i<db->getSize(); i++){   // i starts at 1 because we ignore the root
		State* s = db->getState(i);
		State* p = db->getState(s->getParentID());
		double iFromX = p->getParameter()[x];
		double iFromY = p->getParameter()[y];
		double iToX = s->getParameter()[x];
		double iToY = s->getParameter()[y];
		xmin = minimum(iFromX, iToX, xmin);
		ymin = minimum(iFromY, iToY, ymin);
		xmax = maximum(iFromX, iToX, xmax);
		ymax = maximum(iFromY, iToY, ymax);
		cmdstr << " set arrow from " << iFromX << "," << iFromY << " to " << iToX << "," << iToY << " nohead  lc rgb \"" << color << "\" lw 2 \n";
	}

	stringstream board;
	board << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0 with linespoints lt \"white\" pt 0.01";
	cmdstr << board.str() << "\n " << cmdstr.str();
	return cmdstr.str();
}

string Duplex::drawObjectiveTree(){
	string color = "red";
	stringstream cmdstr;
	int x = settings->lookupInt("plot.x");
	int y = settings->lookupInt("plot.y");
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
	board << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0 with linespoints lt \"white\" pt 0.01";
	cmdstr << board.str() << "\n " << cmdstr.str();
	return cmdstr.str();
}

string Duplex::plotError(){
	stringstream cmdstr;
	cmdstr << "plot [" << 0 << ":" << error.size() << "][" << 0 << ":" << 1.1*error[0] << "] 0 with linespoints lt \"white\" pt 0.01" << endl;
	for (int i = 1; i < error.size(); i++){
		cmdstr << " set arrow from " << i - 1 << "," << error[i - 1] << " to " << i << "," << error[i] << " nohead  lc rgb \"red\" lw 2 \n";
	}
	return cmdstr.str();
}

string Duplex::plotDistance(){
    stringstream cmdstr;
    cmdstr << "plot [" << 0 << ":" << currentDistance.size() << "][" << 0 << ":" << 2.0*currentDistance[0] << "] 0 with linespoints lt \"white\" pt 0.01" << endl;
    for (int i = 1; i < currentDistance.size(); i++){
        cmdstr << " set arrow from " << i - 1 << "," << currentDistance[i - 1] << " to " << i << "," << currentDistance[i] << " nohead  lc rgb \"red\" lw 2 \n";
    }
    return cmdstr.str();
}

string Duplex::draw(){
	if (settings->check("plot.type", "error")){
		return plotError();
	}else if (settings->check("plot.type", "distance")){
		return plotDistance();
	}else if (settings->check("plot.type", "tree.parameter")){
		return drawParameterTree();
	}else if (settings->check("plot.type", "tree.objective")){
		return drawObjectiveTree();
    }else{
        return "";
    }
}

void Duplex::save(boost::property_tree::ptree* ptree){
    ptree->add("duplex.version", 1);
    boost::property_tree::ptree& data = ptree->add("duplex.data", "");
    db->save(&data);
	boost::property_tree::ptree& sense = ptree->add("duplex.sensitivity", "");
	sensitivity->save(&sense);
    boost::property_tree::ptree& node = ptree->add("duplex.stat", "");
	
	for (int i = 0; i<db->getSize(); i++){
        boost::property_tree::ptree& iter = node.add("iteration", "");
        iter.add("id", i);
        iter.add("error", error[i]);
        iter.add("distance", currentDistance[i]);
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
            error.push_back(v.second.get<double>("error"));
            currentDistance.push_back(v.second.get<double>("distance"));
        }
    }
}
