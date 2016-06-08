#pragma once

class Optimizer{
protected:
    
public:
    Optimizer();
    ~Optimizer();
    
    virtual bool hasGradientInformation() = 0;
    virtual void optimize() = 0;
    
    void clipNorm();
    void klDivergence();
    void getState();
    void setState();
    void getGradient();
    void getUpdates();
    void setWeights();
    void getWeights();
    
};