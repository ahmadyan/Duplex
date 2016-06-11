#include "gradientDescent.h"

GradientDescent::GradientDescent(Settings* s):Optimizer(s){
    learning_rate=settings->lookupFloat("optimization.learning_rate");
    decay = settings->lookupFloat("optimization.decay");
    cout << "Initializing Gradient Descent optimizer" << endl ;
}

GradientDescent::~GradientDescent(){
}

bool GradientDescent::hasGradientInformation(){
    return true;
}

State* GradientDescent::update(State* u){
    auto pSize = u->getParameterSize();
    auto objSize = u->getObjectiveSize();
    double* prev = u->getParameter();
    auto iterations = u->getID();
    auto lr = learning_rate * (1.0 / (1.0 + decay * iterations));
    State* v = new State(pSize, objSize);
    double* input = new double[pSize]();
    //buggy, how to model multi-objective functions here?
    for(int i=0;i<u->getParameterSize();i++){
        input[i]= prev[i] - learning_rate*u->getDerivative(0, i);       //descent rule
        //todo: implement clipping code
        if(input[i]<-1) input[i]=-1;
        if(input[i]>1) input[i]=1;
    }
    v->setParameter(input);
    return v;
}
