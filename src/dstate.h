#pragma once
#include <iostream>
#include "state.h"

using namespace std;

class DState {
    int sampleSize;
public:
    vector<vector<double> > centers;
    vector<int> tags;
    vector<double> cost;
    double totalCost;
    
    DState(Settings* s, int);
    DState(DState*);
    ~DState();
    void setParameter(vector<vector<double> >);
    vector<vector<double> > getParameter();
};


class dstateGreaterComparator{
public: bool operator()(const DState* lhs, const DState* rhs) const{
    return lhs->totalCost > rhs->totalCost;
}
};

class dstateLesserComparator{
public: bool operator()(const DState* lhs, const DState* rhs) const{
    return lhs->totalCost < rhs->totalCost;
}
};