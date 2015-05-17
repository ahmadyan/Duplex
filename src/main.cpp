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

int main(int argc, char** argv){
    if(argc<2){
        cout << "Usage: duplex configfile.xml" << endl ;
        return -1;
    }
    Configuration* config = new Configuration(string(argv[1]));
    System* system = new System(config);
    Duplex* duplex = new Duplex(config);

    //vector<double> init = config->getVector<double>("system.parameters", "parameters", "init");
    //vector<double> goal = config->getVector<double>("system.parameters", "parameters", "init");
    double* init = new double[2]; init[0]=0;init[1]=0;
    double* goal = new double[2]; goal[0]=2;goal[1]=5;
    
    duplex->setSystem(system);
    duplex->initialize(init);
    duplex->setObjective(goal);

    duplex->optimize();
    
    delete duplex;
    delete system;
    delete config;
	//cin.get();
    return 0;
}
