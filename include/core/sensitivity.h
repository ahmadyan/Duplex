//
//  sensitivity.h
//  record the changes in parameters and objectives over-time
//	The typical flow for this class is like this, a series of input-change
//pushes, followed by series of output change pushes and then a commit.
//	this flow is repeated in each iteration
//
//	Iteration 1
//		pushBackInputChange(1, 2.5, 0.1)
//		pushBackInputChange(2, 2.5, 0.1)
//		pushBackOutputChange(1, 15, 2)
//		pushBackOutputChange(4, 15, 2)
//		commit()
//	Iteration 2
//		pushBackInputChange(...)
//		pushBackOutputChange(...)
//		pushBackOutputChange(...)
//		commit()
//	...
//	Finally,
//		generateSensitivtyMatrix()
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.

#pragma once
#include <boost/property_tree/ptree.hpp>
#include <vector>
using namespace std;

class Sensitivity {
  int totalQueries;
  int inputSize;
  int outputSize;
  double relativeSensitivityThreshold;
  vector<int> inputStack;
  vector<double> inputDeltaStack;
  vector<double> inputValueStack;

  vector<int> outputStack;
  vector<double> outputDeltaStack;
  vector<double> outputValueStack;

  double *inputMin;
  double *inputMax;
  double *outputMin;
  double *outputMax;

  double **sensitivityMatrix;

public:
  Sensitivity(int, int, double);
  ~Sensitivity();
  void setInputOutputMinMax(double *iMin, double *iMax, double *oMin,
                            double *oMax);
  void pushBackInputChange(int, double, double);
  void pushBackOutputChange(int, double, double);
  void commit();
  void generateSensitivityMatrix();
  void save(boost::property_tree::ptree *ptree);
};
