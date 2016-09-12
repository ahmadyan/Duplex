#include "systemOptimizer.h"

SystemOptimizer::SystemOptimizer(Settings* s):NonconvexOptimizer(s){
    engine = "system-optimizer";
}

SystemOptimizer::~SystemOptimizer(){
}

void SystemOptimizer::optimize(){
    Duplex::optimize();
    if (settings->check("sensitivity-analysis.enable", "true"))
        stat->sensitivity->generateSensitivityMatrix();
}

State* SystemOptimizer::localStep(int i, State* qnear){
    computeTemperature(i);
    nextCandidateParameter = computeNextCandidateParameter(qnear);
    double* input = generateNewInput(qnear);
    State* qnew = new State(parameterDimension, objectiveDimension);
    qnew->setParameter(input);
    qnew->setParent(qnear);
    stat->sensitivity->pushBackInputChange(nextCandidateParameter, qnew->getParameter()[nextCandidateParameter], stepLength);
    return qnew;
}

