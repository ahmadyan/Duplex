//
//  state.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#include <random>
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
		d += ((objectiveVector[i] - a->objectiveVector[i]) * (objectiveVector[i] - a->objectiveVector[i]) / (max[i] - min[i]));
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

void State::save(boost::property_tree::ptree* pt){
    stringstream param, obj, rew;
    for(int i=0;i<parameterDimension;i++) param << parameterVector[i] << " ";
    for(int i=0;i<objectiveDimension;i++) obj << objectiveVector[i] << " ";
    for(int i=0;i<parameterDimension;i++) rew << reward[i] << " ";
    
    pt->put("id", id);
    pt->put("parameter", param.str());
    pt->put("objective", obj.str());
    pt->put("reward", rew.str());
    if(type == StateType::StateTypeRoot )
        pt->put("<xmlattr>.root", true);
}