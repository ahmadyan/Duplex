#include "classification.h"
#include "adadelta.h"
#include "adagrad.h"
#include "adam.h"
#include "gradientDescent.h"
#include "nadam.h"
#include <cmath>

Classifier::Classifier(Settings *s, Data *trainingData)
    : NonconvexOptimizer(s) {
  data = trainingData;
  settings = s;
  featureSize = settings->lookupFloat("data.dimension") -
                1; // 1 counts for the label column
  classSize = settings->lookupFloat("classification.classes");
  sampleSize = settings->lookupFloat("data.size");
  engine = "multi-class logistic regression";
  if (settings->check("optimization.algorithm", "gd")) {
    optimizer = new GradientDescent(settings);
    engine = "descent-optimizer-gradient-descent";
  } else if (settings->check("optimization.algorithm", "adagrad")) {
    optimizer = new Adagrad(settings);
    engine = "descent-optimizer-adagrad";
  } else if (settings->check("optimization.algorithm", "nadam")) {
    optimizer = new Nadam(settings);
    engine = "descent-optimizer-nadam";
  } else if (settings->check("optimization.algorithm", "adam") ||
             settings->check("optimization.algorithm", "adamax")) {
    optimizer = new Adam(settings);
    engine = "descent-optimizer-adam/adamax";
  } else if (settings->check("optimization.algorithm", "adadelta")) {
    optimizer = new Adadelta(settings);
    engine = "descent-optimizer-adadelta";
  } else {
    optimizer = new GradientDescent(settings);
  }
  cout << "Initializing multi-class regression algorithm " << endl
       << ".Features= " << featureSize << endl
       << ".Classes= " << classSize << endl
       << ".Samples " << sampleSize << endl
       << endl;
}

Classifier::~Classifier() {}

double Classifier::sigmoid(double x) {
  double r = 1.0 / (1 + exp(-x));
  return r;
}

double Classifier::dot(double *theta, vector<double> x) {
  double r = 0;
  for (int i = 0; i < parameterDimension; i++) {
    r += theta[i] * x[i];
  }
  return r;
}

double Classifier::logistic(double *theta, vector<double> x) {
  return sigmoid(dot(theta, x));
}

double Classifier::regularizer(double *theta) {
  double r = 0;
  for (int i = 0; i < parameterDimension; i++) {
    r += theta[i] * theta[i];
  }
  return r;
}

// Compute cost and gradient for logistic regression using theta as a parameter
double *Classifier::loss(double *theta) {
  double *result = new double[parameterDimension];
  double r = 0;
  for (int i = 0; i < sampleSize; i++) {
    auto y = data->getLabel(i);
    auto x = data->getData(i);
    auto h = logistic(theta, x);
    r += -y * log(h) - (1 - y) * log(1 - h);
    // auto lambda=0.0;
    // r += lambda*regularizer(theta)/(2*sampleSize);
  }
  r = r / sampleSize;
  result[0] = r;
  return result;
}

double *Classifier::gradient(double *theta) {
  double *grad = new double[parameterDimension];
  for (int i = 0; i < parameterDimension; i++)
    grad[i] = 0;
  for (int i = 0; i < sampleSize; i++) {
    auto x = data->getData(i);
    auto y = data->getLabel(i);
    auto h = logistic(theta, x);
    for (int j = 0; j < parameterDimension; j++) {
      grad[j] += (h - y) * x[j];
    }
  }
  for (int j = 0; j < parameterDimension; j++)
    grad[j] /= sampleSize;
  return grad;
}

State *Classifier::localStep(int i, State *qnear) {
  // theta = theta - (alpha/m) * (X' * (X*theta - y));
  computeTemperature(i);
  // double* input = generateNewInput(qnear);
  auto learning_rate = 0.05; //(1/(1+0.1*i));
  auto theta = qnear->getParameter();
  auto theta_new = new double[parameterDimension]();
  auto grad = qnear->getDerivativeVector(0);
  for (int j = 0; j < qnear->getParameterSize(); j++) {
    auto noise = computeStepLength();
    theta_new[j] = theta[j] - learning_rate * grad[j] + noise;
    cout << "* " << j << " " << theta[j] << " " << theta_new[j]
         << " grad=" << grad[j] << "  noise=" << noise << endl;
  }
  State *qnew = new State(parameterDimension, objectiveDimension);
  qnew->setParameter(theta_new);
  qnew->setParent(qnear);
  return qnew;
}

double Classifier::evaluate(State *qnew) {
  auto theta = qnew->getParameter();
  cout << theta[0] << " " << theta[1] << " " << theta[2] << endl;
  auto cost = loss(theta);
  auto grad = gradient(theta);
  qnew->setObjective(cost);
  qnew->setJacobian(grad);
  score(qnew, max, min);
  return qnew->getObjective()[0];
}

// Overrides the default scoring method (based on L2-distance to the optimum
// solution) with a loss based scoring method, we optimize by lowering the loss
double Classifier::score(State *state, double *maxBound, double *minBound) {
  // objective was computed in evaluate using loss(theta)
  auto score = state->getObjective()[0];
  state->setScore(score);
  double prevScore = optimum ? optimum->getScore() : 9999;
  if (score < prevScore)
    optimum = state;
  return score;
}
