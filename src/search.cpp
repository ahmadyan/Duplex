#include "search.h"

Search::Search(int d){
    dim=d;  //dim is the search dimension
    kd = kd_create(dim);
}

Search::~Search(){
    //delete kd;
}

void Search::insert(State* s){
    kd_insert(kd, s->getObjective(), s);
    db.push_back(s);
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



State* Search::search(State* s){
    struct kdres  *set;
    cout << s->toString() << endl;
    //getting the nearest node
    //struct kdres *kd_nearest(struct kdtree *tree, const double *pos);
    set = kd_nearest(kd, s->getObjective());
    if (kd_res_size(set)>0){
        State* res = (State*)kd_res_item_data(set);
        cout << res->toString() << endl;
        return res;
    }
    else{
        cout << "[error] kd-tree search found 0 closest node." << endl;
        return NULL;
    }
}

void Search::range(State* s){
    struct kdres *presults;
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
