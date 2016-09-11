#pragma once
#include <iostream>
#include <boost/property_tree/ptree.hpp>

#include "configuration.h"
#include "system.h"
#include "search.h"
#include "stat.h"
#include "data.h"

using namespace std;

enum class Temperature { temperatureexp, temperaturefast, temperatureboltz };
enum class Annealing { annealingfast, annealingboltz, annealingfastrandom, annealingboltzrandom};

class Duplex{
    
protected:
    Settings* settings;
    Search* db;
    Stat* stat;
    System* system;
    
    
    //System:
    State* goal;
    double* max;
    double* min;
    double* opt;
    int parameterDimension;
    int objectiveDimension;
    
    //Duplex options
    int iterationCap;
    Temperature temperatureOption;
    Annealing annealingOption;
    double t0;			//initial temperature
    double temperature; //current temperature
    double stepLength;
    double initialStepLength;
    bool reinforcementLearningOption;
    bool shrinkGoalRegionWithTemperateOption;
    int nextCandidateParameter;
    
    //Duplex outputs:
    double minAward;
    double maxAward;
    double delta;
    double* goalRegionBoxMin;
    double* goalRegionBoxMax;
    double* parameterMin;
    double* parameterMax;
    vector<string> objectiveType;
public:
    Duplex(Settings* s);
    ~Duplex();

    // methods for optimizing the function, most of these methods should also be implemented in the inherited classes too
    virtual void initialize()=0;
    void insert(int i, State* qnear, State* qnew);
    virtual State* globalStep()=0;
    virtual State* localStep(int, State*)=0;
    virtual double evaluate(State*)=0;
    virtual bool isConverged(int, State*)=0;
    void optimize();
    void train();
    
    // aux methods, loading, saving, etc.
    void save(boost::property_tree::ptree* ptree);
    void load(boost::property_tree::ptree* ptree);
    Search* getDB();
    System* getSystem();
    void setStat(Stat*);
    Stat* getStat();
    double score(State* state, double* maxBound, double* minBound);
};
