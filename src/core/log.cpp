#include "log.h"
const std::string Log::default_log_header = "#log ";
void Log::setVerbose(bool v){
    verbose = v;
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
void Log::tick(){
    ts_init = clock();
}
    
void Log::tock(std::string processName){
        ts_final = clock();
        ts_total = (double) (ts_final - ts_init) / (double) CLOCKS_PER_SEC ;
        out << endl << "#profile " << processName << " : " << ts_total << " seconds" << endl;
}
