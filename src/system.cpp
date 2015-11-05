#include "system.h"
#include <iostream>
#include <cmath>
#include <string>
using namespace std;

System::System(Settings* s){
	config = s;
	if (config->check("simulation.engine", "hspice")){
		type = HSPICE;
		engine = new Hspice(config);
        
        simulationICFileResult = "sim_ic";
        simulationICFileSource = "sim_ic";
        simulationType = "dc";   //restarts every simulation from time t=0
        simulationLogFilename = config->lookupString("simulation.logfile");
        setting.push_back(simulationICFileResult);
        setting.push_back(simulationICFileSource);
        setting.push_back(simulationType);
        objectives = config->listValues("objective", "uid-objective.keyword");
        parameterName = config->listValues("parameter", "uid-parameter.name");
        parameterUnit = config->listValues("parameter", "uid-parameter.unit");
	}else{
		type = INTERNAL;
	}
}
System::~System(){
}

void System::eval(State* s){
    return eval(s, 0);
}

void System::eval(State* s, double t){
	if (type == HSPICE){
        //int unique_id = s->getID();
        engine->generateNetlist(parameterName, parameterUnit, s->getParameter(), setting);
        engine->runSimulation(simulationLogFilename);
		double* result = engine->parseSimulationLog(simulationLogFilename, objectives);
		s->setObjective(result);
	}
	
	if (type == INTERNAL){
		//f1(x,y) = 31-x1^2 exp(-x1^2 - (x2+1)^2) - 10(x1/5 - x1^3 - x2^5)
		//f2(x,y) = x-y
		double* x = s->getParameter();
		double* j = new double[2];

		//j[0] = pow(x[0], 2) + pow(x[1], 2);
		//j[1] = 10-x[0]*x[1];
		j[0] = x[0]+x[1];
		j[1] = x[0]*x[1];
		s->setObjective(j);
	}
}



