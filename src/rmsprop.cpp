#include "rmsprop.h"

RMSProp::RMSProp(Settings* s):Optimizer(s){
    learning_rate_base=settings->lookupFloat("optimization.learning_rate");
    cout << "Initializing RMSProp optimizer complete"
        << ", learning rate=" << learning_rate_base
        << endl;
}

RMSProp::~RMSProp(){}

bool RMSProp::hasGradientInformation(){
    return true;
}

State* RMSProp::update(State* u){
    auto pSize = u->getParameterSize();
    auto objSize = u->getObjectiveSize();
    auto prev = u->getParameter();
    auto iterations = u->getID();
    
    auto v = new State(pSize, objSize);
    auto input = new double[pSize]();
    
    auto learning_rate = learning_rate_base;
    for(int i=0;i<u->getParameterSize();i++){
        auto dx = u->getDerivative(0, i);
        input[i] = prev[i] - learning_rate*dx;
        if(input[i]<-1) input[i]=-1;
        if(input[i]>1) input[i]=1;
    }
    v->setParameter(input);
    return v;
}
