//
//  state.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//

#include "state.h"

State::State(int _parameterDimension, int _objectiveDimension){
    parameterDimension=_parameterDimension;
    objectiveDimension=_objectiveDimension;
    parameterVector = new double[parameterDimension]();
    objectiveVector = new double[objectiveDimension]();
}

State::~State(){
    delete parameterVector;
    delete objectiveVector;
}
