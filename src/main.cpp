//
//  main.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//

#include <iostream>
#include <string>
#include "configuration.h"
#include "duplex.h"
#include "system.h"
#include "graphics.h"

int main(int argc, char** argv){
	srand((unsigned int)time(0));
	string configFile;
	char full[_MAX_PATH];
	_fullpath(full, ".\\", _MAX_PATH);
	cout << "Current working directory is: " << full << endl;
	if (argc > 2){
		cout << "loading configuration file " << argv[1] << endl;
		configFile = string(full) + "config//" + argv[1];
	}
	else{
		//hard-coded configuration file
		configFile = string(full) + "config.xml";
	}
	Configuration* config = new Configuration(configFile);
    Graphics* graphic = new Graphics(gnuPlot);
    
    System* system = new System(config);
    Duplex* duplex = new Duplex(config);

    //vector<double> init = config->getVector<double>("system.parameters", "parameters", "init");
    //vector<double> goal = config->getVector<double>("system.parameters", "parameters", "init");
    double* init = new double[2]; init[0]=0;init[1]=0;
    double* goal = new double[2]; goal[0]=2;goal[1]=1;
    
    duplex->setSystem(system);
    duplex->initialize(init);
    duplex->setObjective(goal);

    duplex->optimize();
    graphic->execute(duplex->draw());
    graphic->saveToPdf("output.png");
    delete duplex;
    delete system;
    delete config;
	cin.get();
    return 0;
}
