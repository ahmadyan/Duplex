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
	cout << "Duplex initialization ..." << endl;
	settings = c;
	iterationCap = settings->lookupInt("iterations");
	parameterDimension = settings->lookupInt("parameter.size");
	objectiveDimension = settings->lookupInt("objective.size");

	t0 = settings->lookupFloat("initial_temperature");
	temperature = t0;
	initialStepLength = settings->lookupFloat("initial_step_length");
	reinforcementLearningOption = settings->check("reinforcement_learning", "enable");
	if (reinforcementLearningOption){
		minAward = settings->lookupFloat("min_reward");
		maxAward = settings->lookupFloat("max_reward");
		delta = settings->lookupFloat("delta_reward");
	}
	if (settings->check("temperature", "fast")){
		temperatureOption = Temperature::temperaturefast;
	}
	else if (settings->check("temperature", "boltz")){
		temperatureOption = Temperature::temperatureboltz;
	}
	else if (settings->check("temperature", "exp")){
		temperatureOption = Temperature::temperatureexp;
	}
	else{
		cout << "Temperature option not found in the config file. Possible options are [fast, boltz, exp].";
		exit(2);
	}

	if (settings->check("annealing", "fast")){
		annealingOption = Annealing::annealingfast;
	} else if (settings->check("annealing", "boltz")){
		annealingOption = Annealing::annealingboltz;
	} else if (settings->check("annealing", "fast-random")){
		annealingOption = Annealing::annealingfastrandom;
	} else if (settings->check("annealing", "boltz-random")){
		annealingOption = Annealing::annealingboltzrandom;
	}else{
		cout << "Annealing option not found in the config file/ Possible options are [fast, boltz]";
		exit(2);
	}
    
    db = new Search(settings);
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
	double* init = getInitialState();
	double* reward = new double[parameterDimension];
	for (int i = 0; i < parameterDimension; i++)
		reward[i] = 1;
	root = new State(parameterDimension, objectiveDimension);
    root->setParameter(init);
	root->setReward(reward, (double)parameterDimension);
    system->eval(root);

	db->insert(root);
	root->setID(0);
	max = new double[objectiveDimension];
	min = new double[objectiveDimension];
	for (int i = 0; i < objectiveDimension; i++){
		min[i] = -3;
		max[i] = 3;
	}
	error.push_back(goal->distance(root, max, min));
    currentDistance.push_back(goal->distance(root, max, min));
}

void Duplex::setObjective(){
	double* g = new double(objectiveDimension);
    vector<string> objectives = settings->listVariables("objective", "uid-objective");
    for(int i=0;i<objectives.size();i++){
        g[i] = settings->lookupFloat(("objective." + objectives[i] + ".goal").c_str());
    }
    goal = new State(parameterDimension, objectiveDimension);
    goal->setObjective(g);
}

void Duplex::setSystem(System* sys){
    system=sys;
}

State* Duplex::globalStep(){
    //Randomly generate a state close to the final objective
    //todo: use gaussian distribution to generate this
    State* qsample = new State(parameterDimension, objectiveDimension);
    double* min = new double[std::max(parameterDimension, objectiveDimension)];
    double* max = new double[std::max(parameterDimension, objectiveDimension)];
    
    min[0]=1.8; max[0]=0.8;
    min[1]=2.2; max[1]=1.2;
    //for(int i=0;i<objectiveDimension;i++){
    //    min[i]=0;
    //    max[i]=1;
    //}
    qsample->setParameter(qsample->uniformRandomVector(parameterDimension, min, max));
    qsample->setObjective(qsample->uniformRandomVector(objectiveDimension, min, max));
    return qsample;
}

//slightly changes the input
double* Duplex::generateNewInput(State* q){
    int pSize = q->getParameterSize();
    double* param = q->getParameter();
    double* input = new double[pSize];
    for(int j=0;j<pSize;j++) input[j] = param[j];
	input[nextCandidateParameter] += stepLength;
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
}

void Duplex::computeStepLength(){ 
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
}

State* Duplex::localStep(int i, State* qnear){
	computeTemperature(i);
	computeStepLength();
	nextCandidateParameter = computeNextCandidateParameter(qnear);
	double* input = generateNewInput(qnear);
	State* qnew = new State(parameterDimension, objectiveDimension);
	qnew->setParameter(input);
	return qnew;
}

void Duplex::optimize(){
    for(int i=1;i<iterationCap;i++){
		State* qsample = globalStep();              //generate a new bias sample
        State* qnear = db->nearestNode(qsample);        //Find closest node to the objective
		State* qnew = localStep(i, qnear); 
		system->eval(qnew, 0);                  //simulate the circuit with the new input
		update(i, qsample, qnear, qnew);
    }
}

void Duplex::updateError(State* s, double* maxBound, double* minBound){
	double d = goal->distance(s, maxBound, minBound);
	double e = error[error.size() - 1];
	error.push_back(fmin(e, d));
    currentDistance.push_back(d);
}

void Duplex::update(int i, State* qsample, State* qnear, State* qnew){
	db->insert(qnew);                       //add a new node to the database
	qnew->setID(i);
	qnew->setParentID(qnear->getID());      //maintaing the tree data structure
	bias.push_back(qsample);
	updateError(qnew, max, min);
	updateReward(qnear, qnew);
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
	double xmin = 99, xmax = -99, ymin = 99, ymax = -99;
	for (int i = 1; i<db->getSize(); i++){   // i starts at 1 because we ignore the root
		State* s = db->getState(i);
		State* p = db->getState(s->getParentID());
		double iFromX = p->getParameter()[0];//p->getObjective()[0];
		double iFromY = p->getParameter()[1];//p->getObjective()[1];
		double iToX = s->getParameter()[0];//s->getObjective()[0];
		double iToY = s->getParameter()[1];//s->getObjective()[1];
		xmin = minimum(iFromX, iToX, xmin);
		ymin = minimum(iFromY, iToY, ymin);
		xmax = maximum(iFromX, iToX, xmax);
		ymax = maximum(iFromY, iToY, ymax);
		cmdstr << " set arrow from " << iFromX << "," << iFromY << " to " << iToX << "," << iToY << " nohead  lc rgb \"" << color << "\" lw 2 \n";

		/*
		iFromX = 15 + p->getObjective()[0];
		iFromY = p->getObjective()[1] - 8;
		iToX   = 15 + s->getObjective()[0];
		iToY   = s->getObjective()[1] - 8;
		xmin = minimum(iFromX, iToX, xmin);
		ymin = minimum(iFromY, iToY, ymin);
		xmax = maximum(iFromX, iToX, xmax);
		ymax = maximum(iFromY, iToY, ymax);
		cmdstr << " set arrow from " << iFromX << "," << iFromY << " to " << iToX << "," << iToY << " nohead  lc rgb \"" << "red" << "\" lw 2 \n";
		*/
	}

	stringstream board;
	board << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0 with linespoints lt \"white\" pt 0.01";
	//board << " title \"" << title << "\"  \n";
	//board << "set xlabel \"$" << xlabel << "$\" \n";
	//board << "set ylabel \"$" << ylabel << "$\" \n";
	//board << "set zlabel \"$" << zlabel << "$\" \n";
	cmdstr << board.str() << "\n " << cmdstr.str();

	//cout << cmdstr.str() << endl;
	return cmdstr.str();
}

string Duplex::drawObjectiveTree(){
	string color = "red";
	stringstream cmdstr;
	double xmin = 99, xmax = -99, ymin = 99, ymax = -99;
	for (int i = 1; i<db->getSize(); i++){   // i starts at 1 because we ignore the root
		State* s = db->getState(i);
		State* p = db->getState(s->getParentID());
		double iFromX = p->getObjective()[0];//p->getObjective()[0];
		double iFromY = p->getObjective()[1];//p->getObjective()[1];
		double iToX = s->getObjective()[0];//s->getObjective()[0];
		double iToY = s->getObjective()[1];//s->getObjective()[1];
		xmin = minimum(iFromX, iToX, xmin);
		ymin = minimum(iFromY, iToY, ymin);
		xmax = maximum(iFromX, iToX, xmax);
		ymax = maximum(iFromY, iToY, ymax);
		cmdstr << " set arrow from " << iFromX << "," << iFromY << " to " << iToX << "," << iToY << " nohead  lc rgb \"" << color << "\" lw 2 \n";
	}

	stringstream board;
	board << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0 with linespoints lt \"white\" pt 0.01";
	//board << " title \"" << title << "\"  \n";
	//board << "set xlabel \"$" << xlabel << "$\" \n";
	//board << "set ylabel \"$" << ylabel << "$\" \n";
	//board << "set zlabel \"$" << zlabel << "$\" \n";
	cmdstr << board.str() << "\n " << cmdstr.str();
	return cmdstr.str();
}

string Duplex::plotError(){
	stringstream cmdstr;
	cmdstr << "plot [" << 0 << ":" << error.size() << "][" << 0 << ":" << 1.1*error[0] << "] 0 with linespoints lt \"white\" pt 0.01";
	for (int i = 1; i < error.size(); i++){
		cmdstr << " set arrow from " << i - 1 << "," << error[i - 1] << " to " << i << "," << error[i] << " nohead  lc rgb \"red\" lw 2 \n";
	}
	return cmdstr.str();
}

string Duplex::plotDistance(){
    stringstream cmdstr;
    cmdstr << "plot [" << 0 << ":" << currentDistance.size() << "][" << 0 << ":" << 2.0*currentDistance[0] << "] 0 with linespoints lt \"white\" pt 0.01";
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
	}
}


void Duplex::save(boost::property_tree::ptree* ptree){
    ptree->add("duplex.version", 1);
    boost::property_tree::ptree& data = ptree->add("duplex.data", "");
    db->save(&data);

    boost::property_tree::ptree& node = ptree->add("duplex.stat", "");
    for(int i=0;i<iterationCap;i++){
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


