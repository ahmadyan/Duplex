//
//  duplex.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//

#include "duplex.h"
#include "system.h"
#include <algorithm>
#include <iostream>
#include <sstream>
using namespace std;

Duplex::Duplex(Settings* c){
    cout << "Duplex initialization ..." << endl;
	settings = c;
	iterationCap = settings->lookupInt("iterations");
	parameterDimension = settings->lookupInt("parameters");
	objectiveDimension = settings->lookupInt("objectives");
	
	t0 = settings->lookupFloat("initial_temperature");
	temperature = t0;
	if (settings->check("temperature", "fast")){
		temperatureOption = Temperature::temperaturefast;
	}
	else if (settings->check("temperature", "boltz")){
		temperatureOption = Temperature::temperatureboltz;
	}
	else if (settings->check("temperature", "exp")){
		temperatureOption = Temperature::temperatureexp;
	}
	else{
		cout << "Temperature option not found in the config file. Possible options are [fast, boltz, exp].";
		exit(2);
	}

	if (settings->check("annealing", "fast")){
		annealingOption = Annealing::annealingfast;
	}else if (settings->check("annealing", "boltz")){
		annealingOption = Annealing::annealingboltz;
	}else{
		cout << "Annealing option not found in the config file/ Possible options are [fast, boltz]";
		exit(2);
	}

}

Duplex::~Duplex(){
	clear();
}

void Duplex::initialize(double* init){
	root = new State(parameterDimension, objectiveDimension);
    root->setParameter(init);
    system->eval(root);
	db = new Search(objectiveDimension);
	db->insert(root);
	root->setID(0);
	max = new double[objectiveDimension];
	min = new double[objectiveDimension];
	for (int i = 0; i < objectiveDimension; i++){
		min[i] = -3;
		max[i] = 3;

	}
	error.push_back(goal->distance(root, max, min));
}

void Duplex::setObjective(double* g){
	goal = new State(parameterDimension, objectiveDimension);
    goal->setObjective(g);
}

void Duplex::setSystem(System* sys){
    system=sys;
}

double Duplex::distance(){
    return 0;
}

State* Duplex::globalStep(){
    //Randomly generate a state close to the final objective
    //todo: use gaussian distribution to generate this
    State* qsample = new State(parameterDimension, objectiveDimension);
    double* min = new double[std::max(parameterDimension, objectiveDimension)];
    double* max = new double[std::max(parameterDimension, objectiveDimension)];
    
    min[0]=1.8; max[0]=0.8;
    min[1]=2.2; max[1]=1.2;
    //for(int i=0;i<objectiveDimension;i++){
    //    min[i]=0;
    //    max[i]=1;
    //}
    qsample->setParameter(qsample->uniformRandomVector(parameterDimension, min, max));
    qsample->setObjective(qsample->uniformRandomVector(objectiveDimension, min, max));
    return qsample;
}

//slightly changes the input
double* Duplex::generateNewInput(State* q, double temperature){
    int pSize = q->getParameterSize();
    double* param = q->getParameter();
    double* input = new double[pSize];
    
    for(int j=0;j<pSize;j++) input[j] = param[j];
    int candidate = rand()%pSize;   // select an input dimension
    

    double stepLength = 0;
    //stepLength = q->unifRand(-1, 1)*(temperature);     //annealing-linear
    stepLength = q->unifRand(-0.5, 0.5)*(temperature);     //annealing-boltz
    //stepLength = q->unifRand(-0.1, 0.1);              //slow-steps
    
    //How should we change the inputs to the duplex? rapidly or slowly?
    //rapid-changes
    //input[candidate]=q->unifRand(0, 2);       //rapid-changes
    
    
    input[candidate] += stepLength;
    return input;
}

void Duplex::update(int i, State* qsample, State* qnear, State* qnew){
	db->insert(qnew);                       //add a new node to the database
	qnew->setID(i);
	qnew->setParentID(qnear->getID());      //maintaing the tree data structure
	bias.push_back(qsample);
	updateError(qnew, max, min);
}

void Duplex::clear(){
	for (int i = 0; i<bias.size(); i++){
		delete bias[i];
	}
}

void Duplex::computeTemperature(int i){
	switch (temperatureOption){
	case Temperature::temperaturefast:
		temperature = t0 * (1- (1.0*i) / iterationCap );
		break;
	case Temperature::temperatureboltz:
		temperature = t0 / log(i+2);
		break;
	case Temperature::temperatureexp:
		temperature = temperature*0.95;
		break;
	}
}

State* Duplex::localStep(int i, State* qnear){
	computeTemperature(i);
	double* input = generateNewInput(qnear, temperature);
	State* qnew = new State(parameterDimension, objectiveDimension);
	qnew->setParameter(input);
	return qnew;
}


void Duplex::optimize(){
    for(int i=1;i<iterationCap;i++){
		State* qsample = globalStep();              //generate a new bias sample
        State* qnear = db->nearestNode(qsample);        //Find closest node to the objective
		State* qnew = localStep(i, qnear); 
		system->eval(qnew, 0);                  //simulate the circuit with the new input
		update(i, qsample, qnear, qnew);
    }
}

double minimum(double a, double b, double min){
    double m = min;
    if(a<m) m=a;
    if(b<m) m=b;
    return m;
}

double maximum(double a, double b, double max){
    double m = max;
    if(a>m) m=a;
    if(b>m) m=b;
    return m;
}



string Duplex::drawParameterTree(){
    string color="blue";
    stringstream cmdstr;
    double xmin=99, xmax=-99,ymin=99,ymax=-99;
    for(int i=1;i<db->getSize();i++){   // i starts at 1 because we ignore the root
        State* s = db->getState(i);
        State* p = db->getState(s->getParentID());
        double iFromX = p->getParameter()[0];//p->getObjective()[0];
        double iFromY = p->getParameter()[1];//p->getObjective()[1];
        double iToX   = s->getParameter()[0];//s->getObjective()[0];
        double iToY   = s->getParameter()[1];//s->getObjective()[1];
        xmin = minimum(iFromX, iToX, xmin);
        ymin = minimum(iFromY, iToY, ymin);
        xmax = maximum(iFromX, iToX, xmax);
        ymax = maximum(iFromY, iToY, ymax);
        cmdstr << " set arrow from " << iFromX << "," << iFromY << " to " << iToX << "," << iToY << " nohead  lc rgb \"" << color << "\" lw 2 \n";
        
        /*
         iFromX = 15 + p->getObjective()[0];
         iFromY = p->getObjective()[1] - 8;
         iToX   = 15 + s->getObjective()[0];
         iToY   = s->getObjective()[1] - 8;
         xmin = minimum(iFromX, iToX, xmin);
         ymin = minimum(iFromY, iToY, ymin);
         xmax = maximum(iFromX, iToX, xmax);
         ymax = maximum(iFromY, iToY, ymax);
         cmdstr << " set arrow from " << iFromX << "," << iFromY << " to " << iToX << "," << iToY << " nohead  lc rgb \"" << "red" << "\" lw 2 \n";
         */
        
        
    }
    
    stringstream board;
    board << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0 with linespoints lt \"white\" pt 0.01";
    //board << " title \"" << title << "\"  \n";
    //board << "set xlabel \"$" << xlabel << "$\" \n";
    //board << "set ylabel \"$" << ylabel << "$\" \n";
    //board << "set zlabel \"$" << zlabel << "$\" \n";
    cmdstr << board.str()  << "\n " << cmdstr.str() ;
    
    cout << cmdstr.str() << endl;
    return cmdstr.str();
}



string Duplex::drawObjectiveTree(){
    string color="red";
    stringstream cmdstr;
    double xmin=99, xmax=-99,ymin=99,ymax=-99;
    for(int i=1;i<db->getSize();i++){   // i starts at 1 because we ignore the root
        State* s = db->getState(i);
        State* p = db->getState(s->getParentID());
        double iFromX = p->getObjective()[0];//p->getObjective()[0];
        double iFromY = p->getObjective()[1];//p->getObjective()[1];
        double iToX   = s->getObjective()[0];//s->getObjective()[0];
        double iToY   = s->getObjective()[1];//s->getObjective()[1];
        xmin = minimum(iFromX, iToX, xmin);
        ymin = minimum(iFromY, iToY, ymin);
        xmax = maximum(iFromX, iToX, xmax);
        ymax = maximum(iFromY, iToY, ymax);
        cmdstr << " set arrow from " << iFromX << "," << iFromY << " to " << iToX << "," << iToY << " nohead  lc rgb \"" << color << "\" lw 2 \n";
    }
    
    stringstream board;
    board << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0 with linespoints lt \"white\" pt 0.01";
    //board << " title \"" << title << "\"  \n";
    //board << "set xlabel \"$" << xlabel << "$\" \n";
    //board << "set ylabel \"$" << ylabel << "$\" \n";
    //board << "set zlabel \"$" << zlabel << "$\" \n";
    cmdstr << board.str()  << "\n " << cmdstr.str() ;
    return cmdstr.str();
}

string Duplex::plotError(){
	stringstream cmdstr;
	cmdstr << "plot [" << 0 << ":" << error.size() << "][" << 0 << ":" << error[0]+1 << "] 0 with linespoints lt \"white\" pt 0.01";
	for (int i = 1; i < error.size(); i++){
		double y0 = error[i - 1];
		double y1 = error[i];
		cmdstr << " set arrow from " << i-1 << "," << error[i-1] << " to " << i << "," << error[i] << " nohead  lc rgb \"red\" lw 2 \n";
	}
	cout << cmdstr.str() << endl;
	return cmdstr.str();
}

string Duplex::draw(){
	return plotError();
    //return drawParameterTree();
    //return drawObjectiveTree();
}

void Duplex::updateError(State* s, double* maxBound, double* minBound){
	double d = goal->distance(s, maxBound, minBound);
	double e = error[error.size() - 1];
	error.push_back(fmin(e, d));
}