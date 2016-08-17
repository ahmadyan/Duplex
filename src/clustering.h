#pragma once
#include "configuration.h"
#include "data.h"
#include <iostream>
using namespace std;

// The Adagrad optimization algorithm, with support for RMSProp auto-correlation
class Clustering{
    Settings* settings;
    vector<double*> centers;
    vector<int> tags;
    Data* samples;
public:
    Clustering(Settings* s);
    ~Clustering();
    void kmean();
    void clustering();
    double distance(int dim, double* src, double* dst);
    Data* getData();
    vector<int> getTags();
};