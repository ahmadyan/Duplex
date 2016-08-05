#pragma once
#include "configuration.h"

class Data{
protected:
    Settings* settings;
    int dimension;
    string format;
    string sourceFileName;
    vector<double*> payload;
    vector<int> tags;
    
public:
    Data(Settings* setting);
    string toString(int);
    string toString(double*);
    void importData(string, int);
    void exportData(string filename);
    void shuffleData();
    int getSize();
    int getDimension();
    double* getData(int);
    ~Data();
    string plotScatter();
};