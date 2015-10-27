#include "hspice.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <strstream>
#include <string>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <stdio.h>

Hspice::Hspice(Settings* s){
	config = s;
	templateFile = config->lookupString("netlist_template");
	netlistFile = config->lookupString("temporary_netlist");
	dt = config->lookupFloat("simulation_time");
}

Hspice::~Hspice(){
}

void Hspice::generateNetlist(vector<double> parameters, vector<string> setting){
	icfilename = setting[0];
	bool dcSimulation = false;
	if (setting[2].compare("dc") == 0){
		dcSimulation = true;
	}
	stringstream sed;
	sed << "cat " << templateFile << "  | sed ";
	for (int i = parameters.size() - 1; i >= 0; i--){
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

void Hspice::runSimulation(){
	string hspiceCommand = "hspice " + netlistFile + " > Sim.txt";
	system(hspiceCommand.c_str());
}

void Hspice::parseSimulationLog(){
	/*
	//system("cat Sim.txt | grep 5.0000000000n > grep.txt");		// grep the line containing my results at sim time 10ns
	//string line;
	//ifstream simResult("grep.txt");
	//while (simResult.good()){
	//	getline(simResult, line);
	//	if (line.size() > 0){
	//		cout << "Sim: " << line << endl;
	//		result = System::parse(line);
	//		break;
	//	}
	//}
	//simResult.close();

	*/
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