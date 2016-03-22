#include "search.h"
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
Search::Search(Settings* c){
    config=c;
    dim=config->lookupInt("objective.size");    //dim is the search dimension
    kd = kd_create(dim);
}

Search::~Search(){
    //delete kd;
}

void Search::insert(State* s){
    kd_insert(kd, s->getObjective(), s);
	pq.push(s);
    db.push_back(s);
}

State* Search::getOptimum(){
	return pq.top();
}

unsigned long Search::getSize(){
    return db.size();
}

//todo: replace this with operator overload
//State& Search::operator[] (const int i){
//    return *db[i];
//}

State* Search::getState(int i){
    return db[i];
}

void Search::setState(State* s, int i){
    db[i]=s;
}

State* Search::nearestNode(State* s){
    struct kdres  *set;
    //getting the nearest node
    //struct kdres *kd_nearest(struct kdtree *tree, const double *pos);
    set = kd_nearest(kd, s->getObjective());
    if (kd_res_size(set)>0){
        State* res = (State*)kd_res_item_data(set);
        return res;
    }
    else{
        cout << "[error] kd-tree search found 0 closest node." << endl;
        return NULL;
    }
}

void Search::range(State* s){
    //struct kdres *presults;
    //getting the set of nearest node for different time-frames
    //struct kdres *kd_nearest_range(struct kdtree *tree, const double *pos, double range);
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

void Search::save(boost::property_tree::ptree* ptree){
    ptree->add("iterations", getSize());
    for(auto s: db){
        boost::property_tree::ptree& node = ptree->add("node", "");
        s->save(&node);
    }
}

void Search::load(boost::property_tree::ptree* ptree){
    int parameterSize = config->lookupInt("parameter.size");
    int objectiveSize = config->lookupInt("objective.size");
    //int size = ptree->get<int>("iterations");
    auto pnodes = ptree->get_child("");
    for(auto v: pnodes){
        if(v.first=="node"){
            //dim is the search dimension
            State* s = new State(parameterSize, objectiveSize);
            s->load(&v.second);
            insert(s);
        }
    }
}
