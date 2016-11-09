#include "stat.h"
#include <cmath>

Stat::Stat(Settings* s, double* _max, double* _min, double* _opt){
    settings=s;
    max=_max;
    min=_min;
    opt=_opt;
    reinforcementLearningOption = settings->check("reinforcement_learning", "enable");
    shrinkGoalRegionWithTemperateOption = settings->check("shrink_goal_region_with_temperate", "enable");
    if (reinforcementLearningOption){
        minAward = settings->lookupFloat("min_reward");
        maxAward = settings->lookupFloat("max_reward");
        delta = settings->lookupFloat("delta_reward");
    }
    objectiveType = settings->listValues("objective", "uid-objective.goal.mode");
    auto parameterDimension = settings->lookupInt("parameter.size");
    auto objectiveDimension = settings->lookupInt("objective.size");
    sensitivity = new Sensitivity(parameterDimension, objectiveDimension, (double)settings->lookupFloat("sensitivity-analysis.relativeSensitivityThreshold"));
}

Stat::~Stat(){
    for (int i = 0; i<bias.size(); i++){
        delete bias[i];
    }
}

void Stat::updateError(double d){
    auto e = d;
    if(error.size()>0){
        e = fmin(error[error.size() - 1], d);
    }
    error.push_back(e);
    currentDistance.push_back(d);
}

void Stat::updateSensitivity(State* qnear, State* qnew){
    auto obj = qnew->getObjective();
    auto obj_near = qnear->getObjective();
    auto objectiveDimension = qnear->getObjectiveSize();
    for (auto i = 0; i<objectiveDimension; i++){
        double delta = abs(obj[i] - obj_near[i]);
        sensitivity->pushBackOutputChange(i, obj[i], delta);
    }
    sensitivity->commit();
}

void Stat::updateBias(State* q){
    bias.push_back(q);
}

void Stat::update(int i, State* goal, State* qsample, State* qnear, State* qnew, double score){
    updateError(score); //score=score(qnew,goal, max, min)
    updateReward(goal, qnear, qnew);
    updateSensitivity(qnear, qnew);
    bias.push_back(qsample);
}

void Stat::updateConvergence(State* q){
    auto d=1.0;
    if(convergence.size()>0){
        d = q->distance(q->getParent(), max, min);
    }
    convergence.push_back(d);
}

void Stat::updateReward(State* goal, State* qnear, State* qnew){
    auto parameterDimension = goal->getParameterSize();
    double distance = goal->distance(qnew, max, min);
    double pdistance = goal->distance(qnear, max, min);
    double* reward = new double[parameterDimension];
    double* preward = qnear->getRewardVector();
    for (int i = 0; i < parameterDimension; i++){
        reward[i] = preward[i];
    }
    
    double award = (distance - pdistance) / delta;
    if (award < minAward) award = minAward;
    if (award > maxAward) award = maxAward;
    reward[nextCandidateParameter] = +1;
    if (reward[nextCandidateParameter] < minAward) reward[nextCandidateParameter] = minAward;
    if (reward[nextCandidateParameter] > maxAward) reward[nextCandidateParameter] = maxAward;
    
    double sum = reward[nextCandidateParameter] - preward[nextCandidateParameter];
    
    qnew->setReward(reward, qnear->getRewardCDF() + sum);
}

double Stat::getDeltaConvergence(){
    auto size=convergence.size();
    if(size<2){
        return 1.0;
    }else{
        return abs(convergence[size-1]-convergence[size-2]);
    }
}

double Stat::getError(int i){
    return error[i];
}

double Stat::getDistance(int i){
    return currentDistance[i];
}

void Stat::error_push_back(double v){
    error.push_back(v);
}

void Stat::distance_push_back(double v){
    currentDistance.push_back(v);
}

vector<double> Stat::getError(){
    return error;
}

vector<double> Stat::getCurrentDistance(){
    return currentDistance;
}
