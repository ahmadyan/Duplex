//
//  state.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#include <random>
#include <iostream>
#include <sstream>
#include "state.h"

using namespace std;

State::State(int _parameterDimension, int _objectiveDimension){
    parameterDimension=_parameterDimension;
    objectiveDimension=_objectiveDimension;
    parameterVector = new double[parameterDimension]();
	reward          = new double[parameterDimension]();
    objectiveVector = new double[objectiveDimension]();
}

State::~State(){
    delete parameterVector;
    delete objectiveVector;
	delete reward;
}

void State::setObjective(double v, int i){
    objectiveVector[i]=v;
}

void State::setParameter(double* v){
    delete parameterVector;
    parameterVector=v;
}

void State::setObjective(double* v){
    delete objectiveVector;
    objectiveVector=v;
}

double* State::getParameter(){
    return parameterVector;
}

double* State::getObjective(){
    return objectiveVector;
}

StateType State::getType(){
    return type;
}

void State::setType(StateType t){
    type = t;
}

string State::toString(){
	stringstream ss;
	ss << "State []: ";
	for (int i = 0; i < parameterDimension; i++){
		ss << parameterVector[i] << " ";
	}
    ss << " / " ;//endl;
	for (int i = 0; i < objectiveDimension; i++){
		ss << objectiveVector[i] << " ";
	}
	//ss << endl;
	return ss.str();
}

double State::unifRand(){
    return rand() / double(RAND_MAX);
}

double State::unifRand(double a, double b){
    return (b - a)*unifRand() + a;
}

double State::normalRand(double mean, double std, double min, double max){
    std::random_device rd;
    std::mt19937 generator(rd());
    std::normal_distribution<double> normal(mean, std);
    
    double d = normal(generator);
    if (d < min) d = min;
    if (d>max) d = max;
    return d;
}

double* State::uniformRandomVector(int size, double* min, double* max){
    double* data = new double[size];
    for (int i = 0; i<size; i++){
        data[i]=unifRand(min[i], max[i]);
    }
    return data;
}

int State::getParentID(){
    return parentID;
}

void State::setParentID(int pid){
    parentID=pid;
}

void State::setParent(State* s){
	parent = s;
	parentID = s->getID();
}

State* State::getParent(){
	return parent;
}


int State::getID(){
    return id;
}

void State::setID(int i){
    id=i;
}

int State::getParameterSize(){
    return parameterDimension;
}

int State::getObjectiveSize(){
    return objectiveDimension;
}


//compute the norm between current objective state and given state a
double State::distance(State* a, double* max, double* min){
	double d = 0;
	for (int i = 0; i<objectiveDimension; i++){
        auto distance = abs(objectiveVector[i] - a->objectiveVector[i]);
        auto normalizedDistance = distance / (max[i]-min[i]);
        d += normalizedDistance * normalizedDistance ;
	}
	d = sqrt(d);
	return d;
}

//compute the norm between current objective state and given state a
double State::absoluteParameterDistance(State* a){
    double d = 0;
    for (int i = 0; i<parameterDimension; i++){
        auto distance = abs(parameterVector[i] - a->parameterVector[i]);
        d += distance * distance ;
    }
    d = sqrt(d);
    return d;
}

double State::normalizedParameterDistance(State* a, double* max, double* min){
    double d = 0;
    for (int i = 0; i<parameterDimension; i++){
        auto distance = abs(parameterVector[i] - a->parameterVector[i]);
        auto normalizedDistance = distance / (max[i]-min[i]);
        d += normalizedDistance * normalizedDistance ;
    }
    d = sqrt(d);
    return d;
}


//compute the distance between two vectors u and v  state and given state a
double State::distance(int size, double* v, double* u){
	double d = 0;
	for (int i = 0; i<size; i++){
		d += (v[i] - u[i]) * (v[i] - u[i]);
	}
	d = sqrt(d);
	return d;
}

double* State::getRewardVector(){
	return reward;
}

double State::getRewardCDF(){
	return rewardCDF;
}

void State::setReward(double* r, double s){
	reward = r;
	rewardCDF = s;
}

double State::getScore(){
    return score;
}

void State::setScore(double s){
    score=s;
}

void State::save(boost::property_tree::ptree* pt){
    stringstream param, obj, rew;
    for(int i=0;i<parameterDimension;i++) param << parameterVector[i] << " ";
    for(int i=0;i<objectiveDimension;i++) obj << objectiveVector[i] << " ";
    for(int i=0;i<parameterDimension;i++) rew << reward[i] << " ";
    
    pt->put("id", id);
    pt->put("parent", parentID);
    pt->put("parameter", param.str());
    pt->put("objective", obj.str());
    pt->put("reward", rew.str());
    pt->put("score", score);
    if(type == StateType::StateTypeRoot )
        pt->put("<xmlattr>.root", true);
}

vector<string> State::split(const string &s, char delim){
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void State::load(boost::property_tree::ptree* pt){
    id = pt->get<int>("id");
    parentID = pt->get<int>("parent");
    score = pt->get<int>("score");
    vector<string> params = split(pt->get<string>("parameter"), ' ');
    for(int i=0;i<params.size();i++){
        parameterVector[i] = stod(params[i]);
    }

    vector<string> objectives = split(pt->get<string>("objective"), ' ');
    for(int i=0;i<objectives.size();i++){
        objectiveVector[i] = stod(objectives[i]);
    }
    
    vector<string> rew = split(pt->get<string>("reward"), ' ');
    for(int i=0;i<rew.size();i++){
        reward[i] = stod(rew[i]);
    }
    
    parameterDimension = (int)params.size();
    objectiveDimension = (int)objectives.size();
}

double State::getDerivative(int i, int j){
    return jacobian[i][j];
}

vector<double> State::getDerivativeVector(int i){
    return jacobian[i];
}

vector<vector<double> > State::getJacobian(){
    return jacobian;
}

void State::setJacobian(vector<vector<double> > j){
    jacobian=j;
}

// convert the double* grad into vector<double> and pushes it into the jacobian matrix
// assumes only one objective
// deletes the grad array afterward
void State::setJacobian(double* grad){
    vector<double> g;
    for(int i=0;i<parameterDimension;i++)
        g.push_back(grad[i]);
    jacobian.push_back(g);
    delete grad;
}
