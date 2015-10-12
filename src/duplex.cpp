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
using namespace std;

Duplex::Duplex(Configuration* c){
    cout << "Duplex" << endl;
    config=c;
}

Duplex::~Duplex(){
    
}

void Duplex::initialize(double* init){
    root = new State(2,2);
    root->setParameter(init);
    system->eval(root);
}

void Duplex::setObjective(double* g){
    goal = new State(2,2);
    goal->setObjective(g);
}

void Duplex::setSystem(System* sys){
    system=sys;
}

double Duplex::distance(){
    return 0;
}

State* Duplex::generateSample(){
    int parameterDimension=2; int objectiveDimension=2;
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
    //stepLength = q->unifRand(-1, 1)*(1-temperature);     //annealing-linear
    stepLength = q->unifRand(-0.5, 0.5)*(1-temperature);     //annealing-boltz
    //stepLength = q->unifRand(-0.1, 0.1);              //slow-steps
    
    //How should we change the inputs to the duplex? rapidly or slowly?
    //rapid-changes
    //input[candidate]=q->unifRand(0, 2);       //rapid-changes
    
    
    input[candidate] += stepLength;
    return input;
}

void Duplex::optimize(){
    cout << "Executing Duplex" << endl;
    int iterationCap=100;
    int parameterDimension=2; int objectiveDimension=2;
    db = new Search(objectiveDimension);
    db->insert(root);
    root->setID(0);
	double* max = new double[objectiveDimension];
	double* min = new double[objectiveDimension];
	for (int i = 0; i < objectiveDimension; i++){
		min[i] = -3;
		max[i] = 3;
	}
	error.push_back(goal->distance(root, max, min));
    vector<State*> bias;
    
    for(int i=1;i<iterationCap;i++){
        State* qsample = generateSample();              //generate a new bias sample
        State* qnear = db->nearestNode(qsample);        //Find closest node to the objective
        bias.push_back(qsample);
        double temperature = (1.0*i)/iterationCap;
        double* input = generateNewInput(qnear, temperature);
        State* qnew = new State(parameterDimension, objectiveDimension);
        qnew->setParameter(input);
        //Evaluate
        system->eval(qnew, 0);                  //simulate the circuit with the new input
        db->insert(qnew);                       //add a new node to the database
        
        qnew->setID(i);

        qnew->setParentID(qnear->getID());      //maintaing the tree data structure
        
        //choose frontier (closest state to the objective)
            //need a distance function
            //pareto selection in multi-objective optimization
                // I don't need pareto-optimization because I'm breaking the objectives. Eucledean distance in multi-dimension objective function is pareto-optimal
            //fast search method (kd-tree or vp-tree)   --> blog-post about kd-tree vs vp-tree vs other fast search methods, effects in high dimensions
        //generate a new input pattern by slightly changing one of x's inputs
		updateError(qnew, max, min);
    }
    
    //testing
    for(int i=0;i<iterationCap; i++){
        cout << db->getState(i)->toString() << endl ;
    }
    
    //cleaning-up
    for(int i=0;i<bias.size();i++){
        delete bias[i];
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
	error.push_back(min(error[error.size()-1], d));
}