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
#include "stat.h"

enum class Temperature { temperatureexp, temperaturefast, temperatureboltz };
enum class Annealing { annealingfast, annealingboltz, annealingfastrandom, annealingboltzrandom};

class Duplex{
	Settings* settings;
    Search* db;
    Stat* stat;
	
	//System:
	State* root;
	State* goal;
	System* system;
	double* max;
	double* min;
	double* opt;
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
	bool shrinkGoalRegionWithTemperateOption;
	int nextCandidateParameter;
	
    //Duplex outputs:
    
	double minAward;
	double maxAward;
	double delta;
    double* goalRegionBoxMin;
    double* goalRegionBoxMax;
	double* parameterMin;
	double* parameterMax;
	vector<string> objectiveType;
public:
	Duplex(Settings*);
    ~Duplex();
	double* getInitialState();
	void setSystem(System*);
	void initialize();
    void setObjective();
    
    void walkOptimizer();
    void optimize();
	void simulated_annealing();
	void functionalOptimization();
	void treeOptimizer();

	//plotting methods
	string draw(int);
    string drawParameterTree(int x, int y, int z, string sizingPreference, string plotType, string title);
    string drawObjectiveTree(int, int, string);
	string drawTrace(int x, int y, string title);
    string drawCanvas(string function, double xmin, double xmax, double ymin, double ymax, string, string);
    
    void insert(int i, State* qnear, State* qnew);
	double score(State*, double*, double*);

	void clear();
	double* generateNewInput(State* q);
	State* localStep(int i, State*);
	State* globalStep();			
	State* foptGlobalStep();
	
	void computeTemperature(int i);
	double computeStepLength();
	int  computeNextCandidateParameter(State* qnear);
	
    void save(boost::property_tree::ptree* ptree);
    void load(boost::property_tree::ptree* ptree);
};