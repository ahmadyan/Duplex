#include "classification.h"
#include <cmath>

Classifier::Classifier(Settings* s){
    /*learning_rate_base = settings->lookupFloat("optimization.learning_rate");
    fudgeFactor = settings->lookupFloat("optimization.fudge_factor");
    autocorr = settings->lookupFloat("optimization.autocorrelation");
    cout << "Initializing Adagrad optimizer complete"
        << ", learning rate = " << learning_rate_base
        << ", fudge factor = " << fudgeFactor
        << ", auto correlation factor = " << autocorr
        << endl;*/
}

Classifier::~Classifier(){}
