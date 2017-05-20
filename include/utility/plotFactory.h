#pragma once
#include "clustering.h"
#include "configuration.h"
#include "duplex.h"
#include "stat.h"
#include <iostream>

using namespace std;

class PlotFactory {
  Settings *settings;
  string plotStr;
  Clustering *clustering;
  Duplex *duplex;
  Stat *stat;

public:
  PlotFactory(Settings *s, Duplex *, Clustering *);
  ~PlotFactory();
  string getPlot(int);
  string plotError();
  string plotDistance();
  string drawTrace(int x, int y, string title);
  string drawCanvas(string function, double xmin, double xmax, double ymin,
                    double ymax, string plotType, string title);
  string drawParameterTree(int x, int y, int z, string sizingPreference,
                           string plotType, string title);
  string drawObjectiveTree(int x, int y, string title);
  string scatterPlot();
};
