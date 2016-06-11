#include "gradientDescent.h"

GradientDescent::GradientDescent(Settings* s):Optimizer(s){
    learning_rate=settings->lookupFloat("optimization.learning_rate");
    decay = settings->lookupFloat("optimization.decay");
    momentum = settings->lookupFloat("optimization.momentum");
    velocity=0;
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
    // decayed learning rate:
    auto lr = learning_rate * (1.0 / (1.0 + decay * iterations));

    State* v = new State(pSize, objSize);
    double* input = new double[pSize]();
    //buggy, how to model multi-objective functions here?
    for(int i=0;i<u->getParameterSize();i++){
        velocity = momentum*velocity - lr*u->getDerivative(0, i);       //momentum rule
        input[i]= prev[i] + velocity;                                   //descent rule

        if(input[i]<-1) input[i]=-1;
        if(input[i]>1) input[i]=1;
    }
    v->setParameter(input);
    return v;
}
