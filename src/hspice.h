//
//  hspice.h
//  duplex
//
//  Created by Adel Ahmadyan on 10/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once
#include <string>
#include "configuration.h"
#include "state.h"
using namespace std;

class Hspice{
	Settings* config;
	string templateFile;
	string netlistFile;
	string icfilename;
	double dt;
	const int unluckyThirteen = 13;
    int parameterSize;
    int objectiveSize;

public:
	Hspice(Settings* config);
	~Hspice();

	void generateNetlist(double* parameters, vector<string> setting);
	void runSimulation(string);
	double* parseSimulationLog(string, vector<string> objectives);
	vector<double> getFinalState();
	vector<double> parse(string s);
	bool is_only_ascii_whitespace(const std::string& str);
	double unit(char u);
	double* parseICFile(string fileName);
};
