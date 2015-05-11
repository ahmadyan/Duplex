//
//  main.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//

#include <iostream>
#include <string>
#include "configuration.h"
#include "duplex.h"
#include "system.h"
#include "kdtree.h"
#include <random>


double unifRand(){
	return rand() / double(RAND_MAX);
}
double unifRand(double a, double b){
	return (b - a)*unifRand() + a;
}


double normalRand(double min, double max){
	std::random_device rd;
	std::mt19937 generator(rd());
	double mean = (min + max) / 2;
	double std = 0.4;
	std::normal_distribution<double> normal(mean, std);

	double d = normal(generator);
	if (d < min) d = min;
	if (d>max) d = max;
	return d;
}

double* uniformRandomVector(int size, double* min, double* max){
	double* data = new double[size];
	for (int i = 0; i<size; i++){
		//data[i] = normalRand(min[i], max[i]);
		data[i]=unifRand(min[i], max[i]);
	}
	return data;
}
void kdtree_experiment(){
	struct kdtree *kd;
	struct kdres  *set;

	int d1 = 3, d2=6;

	double* min = new double[d2];
	double* max = new double[d2];
	//default values for min-max
	for (int i = 0; i<d2; i++){
		min[i] = 0;
		max[i] = 1;
	}
	kd = kd_create(d2);
	for (int i = 0; i<100; i++){
		State* sample = new State(d1, d2);
		double* parameter = uniformRandomVector(d1, min, max);
		double* objective = uniformRandomVector(d2, min, max);
		sample->setParameter(parameter);
		sample->setObjective(objective);
		kd_insert(kd, sample->getObjective(), sample);
	}


	State* q = new State(d1, d2);
	double* qparameter = uniformRandomVector(d1, min, max);
	double* qobjective = uniformRandomVector(d2, min, max);
	q->setParameter(qparameter);
	q->setObjective(qobjective);

	cout << q->toString() << endl;
	//getting the nearest node
	//struct kdres *kd_nearest(struct kdtree *tree, const double *pos);
	set = kd_nearest(kd, q->getObjective());
	if (kd_res_size(set)>0){
		State* res = (State*)kd_res_item_data(set);
		cout << res->toString() << endl;
	}
	else{
		cout << "[error]" << endl;
	}


	//getting the set of nearest node for different time-frames

	//struct kdres *kd_nearest_range(struct kdtree *tree, const double *pos, double range);

	struct kdres *presults;
	//presults = kd_nearest_range(kd, q_sample->get(), 0.01); 
	//if(kd_res_size(presults)>0){	
	//	while( !kd_res_end( presults ) ) {
	//	// get the data and position of the current result item 
	//	node* res =  (node*) kd_res_item( presults, pos );
	//	cout << res->toString() << endl ;
	//	// compute the distance of the current result from the pt 
	//	dist = sqrt( dist_sq( pt, pos, 3 ) );

	//	// print out the retrieved data 
	//	printf( "node at (%.3f, %.3f, %.3f) is %.3f away and has data=%c\n", 
	//    pos[0], pos[1], pos[2], dist, *pch );

	//	// go to the next entry 
	//	kd_res_next( presults );
	//	}
	//   }else{
	//       cout << "[error] 2" << endl ;
	//   }

}
int main(int argc, char** argv){
    //if(argc<2){
    //    cout << "Usage: duplex configfile.xml" << endl ;
    //    return -1;
    //}
    //Configuration* config = new Configuration(string(argv[1]));
    Configuration* config = new Configuration();
    System* system = new System(config);
    Duplex* duplex = new Duplex(config);

    double* init = new double[2]; init[0]=0;init[1]=0;
    double* goal = new double[2]; goal[0]=2;goal[1]=5;
    
    duplex->setSystem(system);
    duplex->initialize(init);
    duplex->setObjective(goal);

    duplex->optimize();
    
	kdtree_experiment();
    delete duplex;
    delete system;
    delete config;
	cin.get();
    return 0;
}
