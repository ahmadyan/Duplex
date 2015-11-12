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

using namespace std;

using std::chrono::system_clock;
const std::string Log::default_log_header = "#log ";
class Log{
    double initial_time;
    double final_time;
    double total_time;
    bool verbose;
    
    ostream& _out_stream;
    bool         _next_is_begin;
    const string _log_header;
    //prototype for std::endl template function
    using endl_type = ostream&(ostream&);
    
public:
    void setVerbose(bool v);
    
    static const std::string default_log_header;
    
    //Constructor: User passes a custom log header and output stream, or uses defaults.
    Log(const std::string& log_header = default_log_header , std::ostream& out_stream = std::cout) : _log_header( log_header ) , _out_stream( out_stream ) , _next_is_begin( true ) {}
    
    //overloads <<endl
    Log& operator<<(endl_type endl){
        _next_is_begin = true;
        _out_stream << endl;
        return (*this);
    }
    
    template<typename T> Log& operator<< (const T& data){
        auto now        = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t( now ); //Uhhg, C APIs...
        auto now_tm     = std::localtime( &now_time_t ); //More uhhg, C style...
        
        if( _next_is_begin )
            _out_stream << _log_header << "(" << now_tm->tm_hour << ":" << now_tm->tm_min << ":" << now_tm->tm_sec << "): " << data;
        else
            _out_stream << data;
        
        _next_is_begin = false;
        return (*this);
    }
    //tick-tock
    // the tick & tock functions should be used toghether (usually in main() ) to
    // calculate the time used by a specific function.
    // they should be used like this:
    //      utils::tick();
    //        f1();
    //        f2();
    //        f3();
    //	utils::tock("name of process/function");
    void tick(){
        initial_time = clock();
    }
    
    void tock(std::string processName){
        final_time = clock();
        total_time = (double) (final_time - initial_time) / (double) CLOCKS_PER_SEC ;
        cout << endl << "[time-report] Time required to " << processName << " is: " << total_time << " seconds"<<endl;
    }

};



