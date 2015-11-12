//
//  log.h
//  Logging class for Duplex
//
//  Created by Adel Ahmadyan on 5/5/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once

#include <vector>
#include <string>
#include <iostream>

using namespace std;
class Log{
    double initial_time;
    double final_time;
    double total_time;
    bool verbose;
public:
	Log(bool);
	~Log();
    void setVerbose(bool v);
    void log(string str);   // todo: use << overload
    
    //tick-tock
    void tick();
    void tock(std::string processName);
};