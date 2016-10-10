#pragma once
#include <iostream>
#include "duplex.h"
#include "optimizer.h"
#include "configuration.h"
#include "data.h"

using namespace std;

class Classifier: public Duplex{
    Data* data;
    Optimizer* optimizer;
    Settings* settings;
    int featureSize;
    int classSize;
    int sampleSize;
public:
    Classifier(Settings*, Data*);
    ~Classifier();
    
    void initialize();
    double* getInitialState();
    State* globalStep();
    State* localStep(int, State*);
    double evaluate(State*);
    bool isConverged(int, State*);
    double* costFunction(double* theta);
    double sigmoid(double x);
    double dot(double* theta, vector<double>);
    double logistic(double* theta, vector<double>);
    double regularizer(double* theta);
    double* gradient(double* theta);
};
