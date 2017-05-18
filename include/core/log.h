//
//  log.h
//  Logging class for Duplex
//
//  Created by Adel Ahmadyan on 5/5/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once

#include <type_traits>
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <time.h>

using namespace std;

using std::chrono::system_clock;

class Log{
    double ts_init;         //time-stamps for performance profiling
    double ts_final;
    double ts_total;
    bool verbose;
    ostream& out;
    bool newline;
    const string _log_header;
    using endl_type = ostream&(ostream&);       //prototype for std::endl template function
    
public:
    
    static const std::string default_log_header;
    
    Log (const std::string& log_header = default_log_header, std::ostream& out_stream = std::cout)
        : out(out_stream), _log_header(log_header)
    {
        newline = true; 
    }

    //overloads <<endl
    Log& operator<< (endl_type endl) {
        newline = true;
        out << endl;
        return (*this);
    }
    
    template<typename T> Log& operator<< (const T& data){
        auto now        = chrono::system_clock::now();
        auto now_time_t = chrono::system_clock::to_time_t( now );
		struct tm now_tm;
#ifdef _WIN32
		localtime_s(&now_tm, &now_time_t);      //localtime_s is the c++11, older compilers should be OK with localtime_r
#else
        localtime_r(&now_time_t, &now_tm);
#endif
        if(newline) out << _log_header << "(" << now_tm.tm_hour << ":" << now_tm.tm_min << ":" << now_tm.tm_sec << "): " << data;
        else out << data;
        newline = false;
        return (*this);
    }
    
    void setVerbose(bool v);
    void tick();
    void tock(std::string processName);
};


