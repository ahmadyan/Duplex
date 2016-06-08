#include "gradientDescent.h"

GradientDescent::GradientDescent(){
    
}

GradientDescent::~GradientDescent(){
    
}

bool GradientDescent::hasGradientInformation(){
    return true;
}

State* GradientDescent::update(State* u){
    double gamma=0.01;
    auto pSize = u->getParameterSize();
    auto objSize = u->getObjectiveSize();
    double* prev = u->getParameter();
    State* v = new State(pSize, objSize);
    double* input = new double[pSize]();
    //buggy, how to model multi-objective functions here?
    for(int i=0;i<u->getParameterSize();i++){
        input[i]= prev[i] - gamma*u->getDerivative(0, i);
        //todo: implement clipping code
        if(input[i]<-1) input[i]=-1;
        if(input[i]>1) input[i]=1;
        cout << i << "->" << input[i] << " " << prev[i] << endl ;
    }
    v->setParameter(input);
    return v;
}
