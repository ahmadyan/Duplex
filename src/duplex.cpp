//
//  duplex.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//

#include "duplex.h"
#include "system.h"
#include <algorithm>
using namespace std;

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

State* Duplex::generateSample(){
    int parameterDimension=2; int objectiveDimension=2;
    //Randomly generate a state close to the final objective
    //todo: use gaussian distribution to generate this
    State* qsample = new State(parameterDimension, objectiveDimension);
    double* min = new double[std::max(parameterDimension, objectiveDimension)];
    double* max = new double[std::max(parameterDimension, objectiveDimension)];
    for(int i=0;i<objectiveDimension;i++){
        min[i]=0;
        max[i]=1;
    }
    qsample->setParameter(qsample->uniformRandomVector(parameterDimension, min, max));
    qsample->setObjective(qsample->uniformRandomVector(objectiveDimension, min, max));
    return qsample;
}

void Duplex::optimize(){
    cout << "Executing Duplex" << endl;
    int iterationCap=100;
    int parameterDimension=2; int objectiveDimension=2;
    db = new Search(objectiveDimension);
    db->insert(root);
    vector<State*> bias;
    
    for(int i=0;i<iterationCap;i++){
        State* qsample = generateSample();              //generate a new bias sample
        State* qnear = db->nearestNode(qsample);        //Find closest node to the objective
        double* param = qnear->getParameter();
        bias.push_back(qsample);
        
        //Slightly change the input
        double* input = new double[parameterDimension];
        for(int j=0;j<parameterDimension;j++)
            input[j] = param[j];
        int candidate = rand()%parameterDimension;                      // select an input dimension
        input[candidate]=qnear->unifRand(0, 1);
        State* qnew = new State(parameterDimension, objectiveDimension);
        qnew->setParameter(input);
        
        //Evaluate
        system->eval(qnew, 0);                  //simulate the circuit with the new input
        db->insert(qnew);                       //add a new node to the database

        //choose frontier (closest state to the objective)
            //need a distance function
            //pareto selection in multi-objective optimization
                // I don't need pareto-optimization because I'm breaking the objectives. Eucledean distance in multi-dimension objective function is pareto-optimal
            //fast search method (kd-tree or vp-tree)   --> blog-post about kd-tree vs vp-tree vs other fast search methods, effects in high dimensions
        //generate a new input pattern by slightly changing one of x's inputs
        
    }
    
    //testing
    for(int i=0;i<iterationCap; i++){
        cout << db->getState(i)->toString() << endl ;
    }
    
    //cleaning-up
    for(int i=0;i<bias.size();i++){
        delete bias[i];
    }
}