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
	
    Configuration* config = new Configuration(string(argv[1]));
    System* system = new System(config);
    Duplex* duplex = new Duplex(config);
	    
    duplex->setInitialState();
    duplex->setObjectiveState();
    duplex->setSystem(system);
    duplex->optimize();
    
    delete duplex;
    delete config;
    return 0;
}
