#include "hspice.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <strstream>
#include <string>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <regex>
 #include <boost/lexical_cast.hpp>

Hspice::Hspice(Settings* s){
	config = s;
	templateFile = config->lookupString("simulation.template");
	netlistFile = config->lookupString("simulation.netlist");
	dt = config->lookupFloat("simulation.time");
    parameterSize = config->lookupInt("parameter.size");
    objectiveSize = config->lookupInt("objective.size");
}

Hspice::~Hspice(){
}

void Hspice::generateNetlist(double* parameters, vector<string> setting){
	icfilename = setting[0];
	bool dcSimulation = false;
	if (setting[2].compare("dc") == 0){
		dcSimulation = true;
	}
	stringstream sed;
	sed << "cat " << templateFile << "  | sed ";
	for (int i = parameterSize - 1; i >= 0; i--){
		sed << "-e s/$PARAM_" << i << "/" << parameters[i] << "/ ";
	}
	if (dcSimulation){
		sed << "-e s/$tran/" << "\".tran " << dt / 10 << " " << dt << "\"/ ";
		sed << "-e s/$save/" << "\".save type=ic file=" << setting[0] << " level=all time=" << dt << "\"/ ";
	}else{
		sed << "-e s/$tran/" << "\".tran " << dt / 10 << " " << dt << " uic" << "\"/ ";
		sed << "-e s/$save/" << "\".save type=ic file=" << setting[0] << " level=all time=" << dt << "\"/ ";
		sed << "-e s/$load/" << "\".load file=" << setting[1] << "\"/";
	}
	sed << " > " << netlistFile << endl;
	system(sed.str().c_str());
}

void Hspice::runSimulation(string simulationLogFilename){
	string hspiceCommand = "hspice " + netlistFile + " > " + simulationLogFilename;
	system(hspiceCommand.c_str());
}

double* Hspice::parseSimulationLog(string filename, vector<string> objectives){
	double* result = new double[objectiveSize];
    for(int i=0;i<objectiveSize;i++){
        result[i]=-1;
    }
    
    //generate the regex for each objective
    vector<regex> regs;
    for(int i=0;i<objectives.size();i++){
        const string floating_point_regex = "([-+]?[0-9]*\\.?[0-9]+)[eE]([-+]?[0-9]+)?";
        const string whitespace_regex = "^[ \t]*";
        regex  reg(whitespace_regex + objectives[i] + "= "  +floating_point_regex );
        regs.push_back(reg);
    }
    
    //parse the logfile, for each line, check it against all the regular-expressions
	ifstream logfile(filename);
	int lineno = 0;
	string s;
	if (logfile.is_open()){
		while (getline(logfile, s)){
			lineno++;
            for(int i=0;i<regs.size();i++){
                auto res = cmatch{};
                if( regex_search(s.c_str(), res, regs[i])){
                    double significand = boost::lexical_cast<double>(res[1]);
                    double exponent = boost::lexical_cast<double>(res[2]);
                    double value = significand * pow(10, exponent);
                    result[i] = value;
                    cout << lineno << ": " << objectives[i] << " " << value << endl;
                }
            }
		}
		logfile.close();
	}
	return result;
}

vector<double> Hspice::getFinalState(){
	vector<double> result;
	int d = config->lookupInt("dimensions");
	result.push_back(dt);
	double* tmp = parseICFile(icfilename + "0");
	for (int i = 0; i < d; i++){
		result.push_back(tmp[i]);
	}
	return result;
}


//The input is something like this:
//5.0000000000n   226.6820055575m   71.3468319456m  -71.3468319456m
// The first number is transient simulation time, the rest are variables that need to be converted to double
vector<double> Hspice::parse(string s){
	vector<double> result;
	string word;
	string str = s;
	if (s.c_str()[s.length() - 1] == unluckyThirteen){	//guess what this does!
		str = s.substr(0, s.length() - 1);
	}
	stringstream stream(str);
	while (getline(stream, word, ' ')){
		if (!is_only_ascii_whitespace(word)){
			double d;
			stringstream ss(word);
			ss >> d;
			char c = word.c_str()[word.length() - 1];
			d = d*unit(c);
			result.push_back(d);
		}
	}
	return result;
}

bool Hspice::is_only_ascii_whitespace(const std::string& str){
	//bool isOnlyWhiteSpace = false;
	//return str.find_first_not_of (' ') == str.npos)
	std::string::const_iterator  it = str.begin();
	do {
		if (it == str.end()) return true;
	} while (*it >= 0 && *it <= 0x7f && std::isspace(*(it++)));
	// one of these conditions will be optimized away by the compiler,
	// which one depends on whether char is signed or not
	return false;
}

double Hspice::unit(char u){
	switch (u){
	case 'f':
	case 'F':
		return 1e-15;
		break;
	case 'p':
	case 'P':
		return 1e-12;
		break;
	case 'n':
	case 'N':
		return 1e-9;
		break;
	case 'u':
	case 'U':
		return 1e-6;
		break;
	case 'm':
	case 'M':
		return 1e-3;
		break;
	case 'k':
	case 'K':
		return 1e3;
		break;

	default:
		return 1;
	}
}

double* Hspice::parseICFile(string fileName){
	int d = config->lookupInt("dimensions");
	double* result = new double[d];
	string line;
	ifstream simResult(fileName);

	if (simResult.good()){
		do{
			getline(simResult, line);	//skip the headers (usually 12 lines)
		} while (line.compare(".ic") != 0);

		for (int i = 0; i<d; i++){
			getline(simResult, line);
			line = line.substr(line.find_first_of("=") + 1, line.length());
			double value;
			stringstream ss(line);
			ss >> value;
			line.erase(line.find_last_not_of(" \n\r\t") + 1);
			char c = line.c_str()[line.length() - 1];
			value = value*unit(c);
			result[i] = value;
		}
	}
	simResult.close();
	return result;
}