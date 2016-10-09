#pragma once
#include <iostream>
#include <queue>
#include "configuration.h"
#include "data.h"
#include "dstate.h"
using namespace std;

class Clustering{
    Settings* settings;
    vector<vector<double> > centers;
    vector<int> tags;
    Data* samples;
    int sampleSize;
    int k;
    int sampleDimension;
    vector<double> costHistory;
    
    double pdelta;
    double delta;
    bool notConverged;
    int counter;
    int iterations;
    vector<DState*> buffer;
    vector<DState*> states;
    priority_queue<DState*, vector<DState*>, dstateLesserComparator > pq;
    
public:
    Clustering(Settings* s, Data* trainingData);
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
    vector<double> kmeanClusterEnergy();
    
    //kmean prototypes
    void kmeanAssignClusters();
    void kmeanInitialize();
    void kmeanUpdateCenters(DState* q);
    void kmeanCheckConvergence();
    void kmeanClassic();
    void kmeanDuplex();
    
    // kmean+duplex prototypes
    DState* initialize();
    DState* globalStep();
    DState* localStep(DState* qnear);
    void insert(DState* new_elem);
    void evaluate(DState* qnew);
    double cost(DState* q);
    void train(string);
};
