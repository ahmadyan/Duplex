//
//  duplex.h
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once
#include <boost/property_tree/ptree.hpp>
#include "configuration.h"
#include "system.h"
#include "search.h"
#include "sensitivity.h"

enum class Temperature { temperatureexp, temperaturefast, temperatureboltz };
enum class Annealing { annealingfast, annealingboltz, annealingfastrandom, annealingboltzrandom};

class Duplex{
	Settings* settings;
    Search* db;
	Sensitivity* sensitivity;

	//System:
	State* root;
	State* goal;
	System* system;
	double* max;
	double* min;
	int parameterDimension;
	int objectiveDimension;
	
	//Duplex options
	int iterationCap;
	Temperature temperatureOption;
	Annealing annealingOption;
	double t0;			//initial temperature
	double temperature; //current temperature 
	double stepLength;
	double initialStepLength;
	bool reinforcementLearningOption;
	int nextCandidateParameter;
	//Duplex outputs:
    vector<double> currentDistance;
	vector<double> error; //keeps the minimum distance from any node in the tree toward the optimum point (Hopefully converges to 0)
	vector<State*> bias; 
	double minAward;
	double maxAward;
	double delta;
    double* goalRegionBoxMin;
    double* goalRegionBoxMax;
	double* parameterMin;
	double* parameterMax;

public:
	Duplex(Settings*);
    ~Duplex();
	double* getInitialState();
	void setSystem(System*);
	void initialize();
    void setObjective();
	
    void optimize();
	void simulated_annealing();
	string draw();
    string drawParameterTree();
    string drawObjectiveTree();
	string plotError();
    string plotDistance();

	void update(int, State* qsample, State* qnear, State* qnew);		//update the database, biases, rewards, etc.
	void updateSensitivity(State* qnear, State* qnew);
	void updateReward(State* qnear, State* qnew);
	void updateError(State* s, double* max, double* min);

	void clear();
	double* generateNewInput(State* q);
	State* localStep(int i, State*);
	State* globalStep();
	void computeTemperature(int i);
	void computeStepLength();
	int  computeNextCandidateParameter(State* qnear);
	
    void save(boost::property_tree::ptree* ptree);
    void load(boost::property_tree::ptree* ptree);
};