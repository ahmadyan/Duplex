//
//  system.h
//  duplex
//
//  Created by Adel Ahmadyan on 5/5/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once
#include "configuration.h"
#include "state.h"

class System{
public:
	System(Configuration* config);
	~System();
    void eval(State*);
};