#pragma once
#include <iostream>
#include "configuration.h"
#include "clustering.h"
#include "duplex.h"
#include "stat.h"

using namespace std;

class PlotFactory{
    Settings* settings;
    int plotID;
    string plotStr;
    Clustering* clustering;
    Duplex* duplex;
    Stat* stat;
    
public:
    PlotFactory(Settings* s, Stat*, Duplex*, Clustering*);
    ~PlotFactory();
    string getPlot();
    string plotError();
    string plotDistance();
    string drawTrace(int x, int y, string title);
    string drawCanvas(string function, double xmin, double xmax, double ymin, double ymax, string plotType, string title);
    string drawParameterTree(int x, int y, int z, string sizingPreference, string plotType, string title);
    string drawObjectiveTree(int x, int y, string title);
    string scatterPlot();
};
