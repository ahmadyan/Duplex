#include "system.h"
#include <cmath>

System::System(Settings*){}
System::~System(){}


void System::eval(State* s){
    return eval(s, 0);
}

void System::eval(State* s, double t){
    //f1(x,y) = 31-x1^2 exp(-x1^2 - (x2+1)^2) - 10(x1/5 - x1^3 - x2^5)
    //f2(x,y) = x-y
    double* x = s->getParameter();
    double* j = new double[2];

    //j[0] = pow(x[0], 2) + pow(x[1], 2);
    //j[1] = 10-x[0]*x[1];
    j[0] = x[0]+x[1];
    j[1] = x[0]*x[1];
    s->setObjective(j);
}



