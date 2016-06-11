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
    
    // We support two types of numerical simulator. 1) Synopsys HSPICE and 2) our internal simulator
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
        // for using the internal simulator, the function has to be explicitly defined in the config file
		type = INTERNAL;
		functions = config->listValues("objective", "uid-objective.function");
		variables = config->listValues("parameter", "uid-parameter.name");
        
        //check if the function's derivatives are provided
        if(config->check("objective.gradient", "true")){
            //for each objective, list all the derivatives.
            // functions are listed as uid- objects, so they might have numbering embedded in them, like uid-00001-objective
            // The functionVariable is the "true" name of the function in the config file
            auto functionVariablesNames = config->listVariables("objective", "uid-objective");
            for(auto fvariableName: functionVariablesNames){
                auto derivativeVariablesNames = config->listVariables(config->concatScope("objective", fvariableName).c_str(), "uid-grad.derivative");
                vector<string> derivatives;
                for(auto dVariableName:derivativeVariablesNames){
                    string derivate = config->lookupString(config->concatScope("objective", fvariableName, dVariableName).c_str());
                    derivatives.push_back(derivate);
                }
                jacobian.push_back(derivatives);
            }
        }
	}
}

System::~System(){
}

void System::eval(State* s){
    return eval(s, 0);
}

double evaluteExpression(string expression, vector<string> variables, double* values){
    const expression::grammar<double, std::string::const_iterator> eg;
    //const double close_enough = std::numeric_limits<double>::epsilon();
    for (int i=0; i<variables.size(); i++){
        boost::replace_all(expression, variables[i], to_string(values[i]));
    }
    std::string::const_iterator iter = expression.begin();
    std::string::const_iterator end = expression.end();
    double result=0;
    parse(iter, end, eg, result);
    return result;
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
                // todo: needs refactoring, come up with a better way to evaluate integralss
                // idea: put integrals as an operator inside the config file
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
                objectives[i] = evaluteExpression(functions[i], variables, parameters);
                if(jacobian.size()>0){
                    vector<vector<double> > jacobianValues;
                    for(int i=0;i<jacobian.size();i++){
                        vector<double> dval;
                        for(int j=0;j<jacobian[i].size();j++){
                            auto dfidj=evaluteExpression(jacobian[i][j], variables, parameters);
                            dval.push_back(dfidj);
                        }
                        jacobianValues.push_back(dval);
                    }
                    s->setJacobian(jacobianValues);
                }
			}
		}
		s->setObjective(objectives);
	}
}

string System::getFunction(int i){
    return functions[i];
}
