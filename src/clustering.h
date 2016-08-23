#pragma once
#include "configuration.h"
#include "data.h"
#include <iostream>
using namespace std;

// The Adagrad optimization algorithm, with support for RMSProp auto-correlation
class Clustering{
    Settings* settings;
    vector<vector<double> > centers;
    vector<int> tags;
    Data* samples;
    int sampleSize;
    int k;
    int sampleDimension;
    vector<double*> sumDistanceInCluster;
    vector<int> totalSamplesInCluster;
    vector<double> costHistory;
    
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
    double distance(vector<double> src, vector<double> dst);
    double distance(vector<double> src, double* dst);
    Data* getData();
    vector<int> getTags();
    vector<double> getCostHistory();
    double kmeanCostFunction();
    
    //kmean prototypes
    void kmeanAssignClusters();
    void kmeanInitialize();
    void kmeanUpdateCenters();
    void kmeanCheckConvergence();
    void kmeanClassic();
    void train(string);
};