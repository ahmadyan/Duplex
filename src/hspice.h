//
//  hspice.h
//  duplex
//
//  Created by Adel Ahmadyan on 10/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once
#include "configuration.h"
#include "state.h"

class Hspice{
public:
	Hspice(Settings* config);
	~Hspice();

	void generateNetlist();
	void runSimulation(vector<double> parameters, vector<string> settings);
	void parseSimulationLog();
};
