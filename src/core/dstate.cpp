#include "dstate.h"

DState::DState(Settings *s, int sz) {
  sampleSize = sz;
  labels = vector<int>(sampleSize, -1);
}

DState::DState(DState *q) {
  sampleSize = q->sampleSize;
  for (int i = 0; i < q->centers.size(); i++) {
    centers.push_back(vector<double>(q->centers[i]));
  }
  labels = vector<int>(sampleSize, -1);
}

DState::~DState() {}

void DState::setParameter(vector<vector<double>> c) { centers = c; }

vector<vector<double>> DState::getParameter() { return centers; }
