#include "adam.h"
#include <cmath>

Adam::Adam(Settings* s):Optimizer(s){
    
    learning_rate_base = settings->lookupFloat("optimization.learning_rate");
    fudgeFactor = settings->lookupFloat("optimization.fudge_factor");
    exponential_decay_rates_beta1=settings->lookupFloat("optimization.exponential_decay_rates_beta1");;
    exponential_decay_rates_beta2=settings->lookupFloat("optimization.exponential_decay_rates_beta2");;
    bias_correction=settings->check("optimization.bias_correction", "true");
    moment = new double[parameterDimension]();
    velocity = new double[parameterDimension]();
    u = new double[parameterDimension]();
    decay_beta1=exponential_decay_rates_beta1;
    decay_beta2=exponential_decay_rates_beta2;
    adamax = settings->check("optimization.algorithm", "adamax");
    cout << "Initializing Adam optimizer complete"
        << ", learning rate = " << learning_rate_base
        << ", exponential_decay_rates_beta1 = " << exponential_decay_rates_beta1
        << ", exponential_decay_rates_beta2 = " << exponential_decay_rates_beta2
        << ", fudge factor = " << fudgeFactor
        << ", bias correction= " << bias_correction
        << ", adamax= " << adamax
        << endl;
    
}

Adam::~Adam(){}

bool Adam::hasGradientInformation(){
    return true;
}

void Adam::update_adam(double* input){
    //lr_t = self.lr * K.sqrt(1. - K.pow(self.beta_2, t)) / (1. - K.pow(self.beta_1, t))
    auto learningRate = learning_rate_base * sqrt(1 - decay_beta2) / (1 - decay_beta1);
    
    //Update biased first and second moment estimate
    //m = beta1*m + (1-beta1)*dx
    //v = beta2*v + (1-beta2)*(dx**2)
    for(int i=0;i<parameterDimension;i++){
        moment[i] = exponential_decay_rates_beta1*moment[i] + (1-exponential_decay_rates_beta1)*dx[i];
        velocity[i] = exponential_decay_rates_beta2*velocity[i] + (1-exponential_decay_rates_beta2)*dx[i]*dx[i];
    }
    
    //x += - learning_rate * m / (np.sqrt(v) + eps)
    for(int i=0;i<parameterDimension;i++){
        auto m=moment[i];
        auto v=velocity[i];
        if(bias_correction){
            m = moment[i] / (1-decay_beta1);
            v = velocity[i]/(1-decay_beta2);
        }
        input[i] = prev[i] - learningRate * m / (sqrt(v)+fudgeFactor);
    }
}

void Adam::update_adamax(double* input){
    auto learningRate = learning_rate_base / (1-decay_beta1);
    //m_t = (self.beta_1 * m) + (1. - self.beta_1) * g
    //u_t = K.maximum(self.beta_2 * u, K.abs(g))
    for(int i=0;i<parameterDimension;i++){
        moment[i] = exponential_decay_rates_beta1*moment[i] + (1-exponential_decay_rates_beta1)*dx[i];
        //velocity is the variable u in the paper, Update the exponentially weighted infinity norm
        velocity[i] = max(exponential_decay_rates_beta2* velocity[i], abs(dx[i]));
    }
    for(int i=0;i<parameterDimension;i++){
        input[i] = prev[i] - learningRate * moment[i] / (velocity[i]+fudgeFactor);
    }
}

State* Adam::update(State* u){
    iterations++;
    prev = u->getParameter();
    auto v = new State(parameterDimension, objectiveDimension);
    auto input = new double[parameterDimension]();
    decay_beta1 *= exponential_decay_rates_beta1;
    decay_beta2 *= exponential_decay_rates_beta2;
    //Get gradients w.r.t. stochastic objective
    dx = u->getDerivativeVector(0);
    if(adamax){
        update_adamax(input);
    }else{
        update_adam(input);
    }
    clipParameters(input);
    v->setParameter(input);
    return v;
}
