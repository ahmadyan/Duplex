//
//  duplex.h
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once
#include "configuration.h"
#include "system.h"
#include "search.h"

enum class Temperature { temperatureexp, temperaturefast, temperatureboltz };
enum class Annealing { annealingfast, annealingboltz, annealingfastrandom, annealingboltzrandom};

class Duplex{
	Settings* settings;
    Search* db;

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
	vector<double> error; //keeps the minimum distance from any node in the tree toward the optimum point (Hopefully converges to 0)
	vector<State*> bias; 
	double minAward;
	double maxAward;
	double delta;
public:
	Duplex(Settings*);
    ~Duplex();
    void initialize(double*);
    void setObjective(double*);
	
    void optimize();
    void setSystem(System*);   
    string draw();
    double* generateNewInput(State* q);
    string drawParameterTree();
    string drawObjectiveTree();
	void updateError(State* s, double* max, double* min);
	string plotError();
	void update(int, State* qsample, State* qnear, State* qnew);		//update the database, biases, rewards, etc.
	void clear();
	State* localStep(int i, State*);
	State* globalStep();
	void computeTemperature(int i);
	void computeStepLength();
	int Duplex::computeNextCandidateParameter(State* qnear);
	void updateReward(State* qnear, State* qnew);
};