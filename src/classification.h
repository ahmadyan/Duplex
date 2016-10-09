#pragma once
#include "configuration.h"
#include "data.h"
#include <iostream>
using namespace std;

class Classifier{
public:
    Classifier(Settings*, Data*);
    ~Classifier();
    
    void train(string mode);
    void trianLogisticRegression();
};
