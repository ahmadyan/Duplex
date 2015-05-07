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

void Duplex::optimize(){
}