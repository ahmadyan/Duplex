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
    int sampleSize;
    int k;
    int sampleDimension;
    vector<double*> sumDistanceInCluster;
    vector<int> totalSamplesInCluster;
    
    double pdelta;
    double delta;
    bool notConverged;
    int counter;
    int maxCounter;
public:
    Clustering(Settings* s);
    ~Clustering();
    void kmean();
    void clustering();
    double distance(int dim, double* src, double* dst);
    Data* getData();
    vector<int> getTags();
    
    //kmean prototypes
    void kmeanAssignClusters();
    void kmeanInitialize();
    void kmeanUpdateCenters();
    void kmeanCheckConvergence();
    void kmeanClassic();
};