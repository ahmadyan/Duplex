#include "hspice.h"
#include <cmath>

Hspice::Hspice(Settings*){}
Hspice::~Hspice(){}



void Hspice::generateNetlist(){

}

void Hspice::runSimulation(vector<double> parameters, vector<string> settings){
  vector<double> result;
/*
	bool dcSimulation = false;
	if (setting[2].compare("dc") == 0){
		dcSimulation = true;
	}

	string templateFile = config->get("edu.uiuc.csl.core.options.template");
	string netlistFile = config->get("edu.uiuc.csl.core.options.netlist");

	stringstream sed;
	sed << "cat " << templateFile << "  | sed ";
	for (int i = param.size()-1; i >=0; i--){
		sed << "-e s/$PARAM_" << i << "/" << param[i] << "/ ";
	}
	if (dcSimulation){
		sed << "-e s/$tran/" << "\".tran " << dt/10 << " " << dt << "\"/ ";
		sed << "-e s/$save/" << "\".save type=ic file=" << setting[0] << " level=all time=" << dt << "\"/ ";
	}else{
		sed << "-e s/$tran/" << "\".tran " << dt/10 << " " << dt << " uic" << "\"/ ";
		sed << "-e s/$save/" << "\".save type=ic file=" << setting[0] << " level=all time=" << dt << "\"/ ";
		sed << "-e s/$load/" << "\".load file=" << setting[1] << "\"/";
	}
	sed << " > " << netlistFile << endl;

	//cout << sed.str() << endl;
	system(sed.str().c_str());

	string hspiceCommand = "hspice " + netlistFile + " > Sim.txt";

	//cout << hspiceCommand << endl;
	system(hspiceCommand.c_str());


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
	result.push_back(dt);
	double* tmp = parseICFile(setting[0] + "0");
	for (int i = 0; i < getDimension(); i++){
		result.push_back(tmp[i]);
	}
	return result;
*/
}

void Hspice::parseSimulationLog(){

}
