#include "adagrad.h"
#include <cmath>

Adagrad::Adagrad(Settings* s):Optimizer(s){
    parameterDimension = settings->lookupInt("parameter.size");
    objectiveDimension = settings->lookupInt("objective.size");
    learning_rate_base = settings->lookupFloat("optimization.learning_rate");
    fudgeFactor = settings->lookupFloat("optimization.fudge_factor");
    
    cout << "Initializing Adagrad optimizer complete"
        << ", learning rate = " << learning_rate_base
        << ", fudge factor = " << fudgeFactor
        << endl;
    cache = new double[parameterDimension]();
}

Adagrad::~Adagrad(){}

bool Adagrad::hasGradientInformation(){
    return true;
}

State* Adagrad::update(State* u){
    auto prev = u->getParameter();
    auto v = new State(parameterDimension, objectiveDimension);
    auto input = new double[parameterDimension]();
    
    auto learningRate = learning_rate_base;
    auto dx = u->getDerivativeVector(0);
    
    // cache += dx**2
    // x += - learning_rate * dx / (np.sqrt(cache) + eps)
    for(int i=0;i<parameterDimension;i++){
        cache[i] += dx[i]*dx[i];
    }
    for(int i=0;i<parameterDimension;i++){
        input[i] = prev[i] - learningRate * dx[i] / sqrt(cache[i]+fudgeFactor);
        if(input[i]<-1) input[i]=-1;
        if(input[i]>1) input[i]=1;
    }
    v->setParameter(input);
    return v;
}
