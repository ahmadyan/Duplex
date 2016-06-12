//
//  state.h
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once
#include <vector>
#include <iostream>
#include "configuration.h"
#include <boost/property_tree/ptree.hpp>

using namespace std;
enum class StateType {StateTypeNormal, StateTypeRoot, StateTypeObjective};

class State{
    int id;
    int parentID;
	State* parent;
    int parameterDimension;
    int objectiveDimension;
    double* parameterVector;
    double* objectiveVector;
    StateType type;
	double* reward;	//used for reinforcement learning
	double rewardCDF;
    double score;
    vector<vector<double> > jacobian;
public:

    State(int, int);
    ~State();
    void setObjective(double v, int i);
    void setParameter(double* v);
    void setObjective(double* v);
    double* getParameter();
    double* getObjective();
    StateType getType();
    void setType(StateType);
	string toString();
    double unifRand();
    double unifRand(double a, double b);
    double normalRand(double mean, double std, double min, double max);
    double* uniformRandomVector(int size, double* min, double* max);
    void setParentID(int);
    int getParentID();
	void setParent(State*);
	State* getParent();
    void setID(int);
    int getID();
    int getObjectiveSize();
    int getParameterSize();
    vector<vector<double> > getJacobian();
    void setJacobian(vector<vector<double> >);
    double getDerivative(int i, int j);
    vector<double> getDerivativeVector(int i);
    
    //distance methods
	double distance(State* a, double* max, double* min);
	double distance(int size, double* v, double* u);
    double absoluteParameterDistance(State* a);
    double normalizedParameterDistance(State* a, double* max, double* min);
    
	double* getRewardVector();
	double  getRewardCDF();
	void setReward(double*, double);
    vector<string> split(const string &s, char delim);
    void save(boost::property_tree::ptree*);
    void load(boost::property_tree::ptree*);
    

    double getScore();
    void setScore(double);
};

class stateGreaterComparator{
public: bool operator()(State* lhs, State* rhs) const{
	return lhs->getScore() > rhs->getScore();
}
};

class stateLesserComparator{
public: bool operator()(State* lhs, State* rhs) const{
	return lhs->getScore() < rhs->getScore();
}
};