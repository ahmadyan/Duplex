#pragma once
#include "configuration.h"

class Data {
protected:
  Settings *settings;
  int dimension;
  string format;
  string sourceFileName;

  vector<vector<double>> payload;

  bool bias;
  // labels:
  vector<int> labels;
  bool hasLabel;
  int labelColumn;

public:
  Data(Settings *setting);
  string toString(int);
  string toString(double *);
  string toString(vector<double> s);
  void importData(string, int);
  void exportData(string filename);
  void shuffleData();
  int getSize();
  int getDimension();
  vector<double> getData(int);
  void setLabel(int, int);
  int getLabel(int);
  ~Data();
  string plotScatter();
};
