//
//  duplex.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//

#include "duplex.h"
#include "system.h"

Duplex::Duplex(Configuration* c){
    cout << "Duplex" << endl;
    config=c;
}


Duplex::~Duplex(){
    
}

void Duplex::initialize(double* init){
    root = new State(2,2);
    root->setParameter(init);
    system->eval(root);
}

void Duplex::setObjective(double* g){
    goal = new State(2,2);
    goal->setObjective(g);
}

void Duplex::setSystem(System* sys){
    system=sys;
}

double Duplex::distance(){
    return 0;
}

void Duplex::optimize(){
    cout << "Executing Duplex" << endl;
    int iterationCap=100;
    vector<State*> states;
    states.push_back(root);
    for(int i=0;i<iterationCap;i++){
        State* near;
        State* sample;
        
        double min_distance = distance();
        int closest_node = 0;
        for(int j=0;j<states.size();j++){
            
        }
        near = states[closest_node];
        //choose frontier (closest state to the objective)
            //need a distance function
            //pareto selection in multi-objective optimization
                // I don't need pareto-optimization because I'm breaking the objectives. Eucledean distance in multi-dimension objective function is pareto-optimal
            //fast search method (kd-tree or vp-tree)   --> blog-post about kd-tree vs vp-tree vs other fast search methods, effects in high dimensions
        //generate a new input pattern by slightly changing one of x's inputs
        
        //simulate the new circuit
    }
    
    for(int i=0;i<iterationCap; i++){
        //print the nodes
    }
}