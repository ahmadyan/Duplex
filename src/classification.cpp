#include "classification.h"
#include "gradientDescent.h"
#include "adagrad.h"
#include "adam.h"
#include "adadelta.h"
#include "nadam.h"
#include <cmath>

Classifier::Classifier(Settings* s, Data* trainingData):Duplex(s){
    data = trainingData;
    settings = s;
    featureSize = settings->lookupFloat("data.dimension") - 1; // 1 counts for the label column
    classSize = settings->lookupFloat("classification.classes");
    sampleSize = settings->lookupFloat("data.size");
    engine = "multi-class logistic regression";
    optimizer = new GradientDescent(settings);
    cout << "Initializing multi-class regression algorithm " << endl
        << ".Features= " << featureSize << endl
        << ".Classes= " << classSize << endl
        << ".Samples " << sampleSize << endl
        << endl;
}

Classifier::~Classifier(){}

double Classifier::sigmoid(double x){
    double r  = 1.0/(1+exp(-x));
    return r;
}

double Classifier::dot(double* theta, vector<double> x){
    double r=0;
    for(int i=0;i<parameterDimension;i++){
        r += theta[i]*x[i];
    }
    return r;
}

double Classifier::logistic(double* theta, vector<double> x){
    return sigmoid(dot(theta, x));
}

double Classifier::regularizer(double* theta){
    double r=0;
    for(int i=0;i<parameterDimension;i++){
        r += theta[i]*theta[i];
    }
    return r;
}

// Compute cost and gradient for logistic regression using theta as a parameter
double* Classifier::costFunction(double* theta){
    double* result = new double[parameterDimension];
    double r = 0;
    for(int i=0;i<sampleSize;i++){
        auto y = data->getLabel(i);
        auto x = data->getData(i);
        auto h = logistic(theta, x);
        r += -y*log(h) - (1-y)*log(1-h);
        // auto lambda=0.0;
        // r += lambda*regularizer(theta)/(2*sampleSize);
    }
    r = r/sampleSize;
    result[0]=r;
    return result;
}

// this gradient implementation is buggy
double* Classifier::gradient(double* theta){
    auto dim=data->getDimension();
    double* grad = new double[dim];
    for(int i=0;i<dim;i++) grad[i]=0;
    for(int i=0;i<sampleSize;i++){
        auto x = data->getData(i);
        auto y = data->getLabel(i);
        auto h = logistic(theta, x);
        for(int j=0;j<dim;j++){
            grad[j] += (h - y) * x[j];
        }
    }
    for (int j=0;j<dim;j++)
        grad[j] /= sampleSize;
    return grad;
}

void Classifier::initialize(){
    //Setting up the root node
    State* root = new State(parameterDimension, objectiveDimension);
    double* theta = new double[parameterDimension];
    for(int i=0;i<parameterDimension;i++){
        theta[i]=0;
    }
    root->setParameter(theta);
    root->setID(0);
    root->setParentID(-1);
    root->setObjective(costFunction(theta));
    cout << costFunction(theta)[0] << endl ;
    cout << "gradient information" << endl ;

    auto g = gradient(theta);
    for(int i=0;i<parameterDimension;i++){
        cout << g[i] << endl ;
        // -0.100000
        // -12.009217
        // -11.262842
    }
    
    vector<vector<double> > jac;
    vector<double> grad;
    for(int i=0;i<data->getDimension();i++)
        grad.push_back(g[i]);
    jac.push_back(grad);
    root->setJacobian(jac);
    delete g;
    
    
    insert(0, root, root);
    
    max = new double[objectiveDimension];
    min = new double[objectiveDimension];
    vector<string> objectiveMinStringVector = settings->listValues("objective", "uid-objective.min");
    vector<string> objectiveMaxStringVector = settings->listValues("objective", "uid-objective.max");
    for (int i = 0; i<objectiveDimension; i++){
        min[i] = stod(objectiveMinStringVector[i]);
        max[i] = stod(objectiveMaxStringVector[i]);
    }
    stat = new Stat(settings, max, min, opt);
    
    // for(int i=0;i<sampleSize;i++){
    //    cout << data->toString(i) << endl ;
    //    cout << data->getLabel(i) << endl ;
    // }
}

State* Classifier::globalStep(){
    return db->getState();
}

State* Classifier::localStep(int i, State* qnear){
    return optimizer->update(qnear);
}

double Classifier::evaluate(State* qnew){
    auto x = qnew->getParameter();
    for(int i=0;i<parameterDimension;i++){
        cout << x[i] << " ," ;
    }cout << "" << endl ;
    auto theta = qnew->getParameter();
    auto cost = costFunction(theta);
    auto grad = gradient(theta);
    qnew->setObjective(cost);
    vector<vector<double> > jac;
    vector<double> g;
    for(int i=0;i<data->getDimension();i++)
        g.push_back(grad[i]);
    jac.push_back(g);
    qnew->setJacobian(jac);
    delete grad;
    return qnew->getObjective()[0];
}

bool Classifier::isConverged(int iteration, State* q){
    stat->updateConvergence(q);
    auto epsilon = settings->lookupFloat("optimization.epsilon");
    //return ( (stat->getDeltaConvergence()>epsilon) && iteration<iterationCap );
    return ( iteration<iterationCap );
}
