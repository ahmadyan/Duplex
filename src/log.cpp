#include "log.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
using namespace std;

Log::Log(bool v){
    verbose=v;
}

Log::~Log(){}

void Log::setVerbose(bool v){
    verbose = v;
}

void Log::log(string str){
    if (verbose){
        cout << str << endl;
    }
}


/*
	the tick & tock functions are used toghether (usually in main() ) to
	calculate the time used by a specific function.
	they should be used like this:
	utils::tick();
        f1();
        f2();
        f3();
	utils::tock("name of process/function");
 */


void Log::tick(){
    initial_time = clock();
}

void Log::tock(std::string processName){
    final_time = clock();
    total_time = (double) (final_time - initial_time) / (double) CLOCKS_PER_SEC ;
    cout << endl << "[time-report] Time required to " << processName << " is: " << total_time << " seconds"<<endl;
}