//
//  system.h
//  duplex
//
//  Created by Adel Ahmadyan on 5/5/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once
#include "configuration.h"
#include "state.h"
#include "hspice.h"
enum simulatorType { HSPICE, GSL, MATLAB, INTERNAL };

class System{
	Settings* config;
	simulatorType type;
	Hspice* engine;
    
    //parameters for hspice simulations
    string simulationICFileResult ;
    string simulationICFileSource ;
    string simulationType;
    string simulationLogFilename;
    vector<string> setting;
    vector<string> objectives;
    vector<string> parameterName;
    vector<string> parameterUnit;
    
	//for internal simulator
	int parameterSize;
	int objectiveSize; 
	vector<string> functions;
	vector<string> variables;
public:
	System(Settings* config);
	~System();
    void eval(State*, double t);
    void eval(State*);
};