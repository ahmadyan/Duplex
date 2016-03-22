#include "system.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include <boost/algorithm/string/replace.hpp>
#include "expression.h"
using namespace std;

System::System(Settings* s){
	config = s;
	parameterSize = config->lookupInt("parameter.size");
	objectiveSize = config->lookupInt("objective.size");

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
		functions = config->listValues("objective", "uid-objective.function");
		variables = config->listValues("parameter", "uid-parameter.name");
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
		double* parameters = s->getParameter();
		double* objectives = new double[objectiveSize];
		for (int i = 0; i < objectiveSize; i++){
			if (functions[i].at(0) == '#'){
				if (s->getParentID() == -1){	
					//no parent node implies root state
					objectives[i] = 0;
				}else{
					if (functions[i] == "#L"){
						// #L is hardcoded for lentgh of the trace
						double fprev = s->getParent()->getObjective()[i];
						double dL = s->distance(s->getParameterSize(), parameters, s->getParent()->getParameter());
						objectives[i] = fprev + dL;
					}
					else if (functions[i] == "#A"){
						//#A is hardcoded for the area of the curve
						double fprev = s->getParent()->getObjective()[i];
						double dx = abs(s->getParent()->getParameter()[0] - s->getParameter()[0]);
						double y = abs(s->getParameter()[1]);
						objectives[i] = fprev + y*dx;
					}
				}
			}else{
				const expression::grammar<double, std::string::const_iterator> eg;
				const double close_enough = std::numeric_limits<double>::epsilon();
				std::string expression = functions[i];
				for (int j = 0; j < parameterSize; j++){
					boost::replace_all(expression, variables[j], to_string(parameters[j]));
				}
				std::string::const_iterator iter = expression.begin();
				std::string::const_iterator end = expression.end();
				double result;
				parse(iter, end, eg, result);
				objectives[i] = result;
			}
		}
		s->setObjective(objectives);
	}
}



