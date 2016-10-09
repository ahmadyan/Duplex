#include "clustering.h"
#include <cmath>
#include <sstream>
#include <fstream>

Clustering::Clustering(Settings* s, Data* trainingData){
    settings = s;
    samples = trainingData;
    sampleSize = samples->getSize();
    k = settings->lookupInt("clustering.clusters");
    sampleDimension = samples->getDimension();
    iterations = s->lookupInt("iterations");
}

Clustering::~Clustering(){
}

double Clustering::distance(int dim, double* src, double* dst){
    double d=0;
    for(int i=0;i<dim;i++){
        d += (src[i]-dst[i])*(src[i]-dst[i]);
    }
    return sqrt(d);
}

double Clustering::distance(vector<double> src, vector<double> dst){
    double d=0;
    for(int i=0;i<src.size();i++){
        d += (src[i]-dst[i])*(src[i]-dst[i]);
    }
    return sqrt(d);
}

double Clustering::distance(vector<double> src, double* dst){
    double d=0;
    for(int i=0;i<src.size();i++){
        d += (src[i]-dst[i])*(src[i]-dst[i]);
    }
    return sqrt(d);
}

Data* Clustering::getData(){
    return samples;
}

vector<int> Clustering::getTags(){
    return tags;
}

vector<double> Clustering::getCostHistory(){
    return costHistory;
}

void Clustering::kmean(){
    cout << "Executing standard kmean clustering algorithm" << endl ;
    vector<double*> sumDistanceInCluster;
    vector<int> totalSamplesInCluster(k, 0);
    for(int i=0;i<k;i++){
        sumDistanceInCluster.push_back(new double[sampleDimension]());
    }
    //initializing centers
    for(int i=0;i<k;i++){
        auto s = samples->getData(i);
        vector<double> c(s);
        centers.push_back(c);
    }
    tags = vector<int>(sampleSize, -1);
    
    auto pdelta = 0.0;
    auto delta = 0.0;
    bool notConverged = true;
    int counter=0;
    const int maxCounter=100;
    while(notConverged){
        // Attribute the closest cluster to each data point
        for(int i=0;i<sampleSize;i++){
            double min_distance=numeric_limits<double>::max();
            for(int j=0;j<k;j++){
                auto dist = distance(samples->getData(i), centers[j]);
                if(dist<min_distance){
                    min_distance=dist;
                    tags[i]=j;
                }
            }
        }
        
        // Set the position of each cluster to the mean of all data points belonging to that cluster
        fill(totalSamplesInCluster.begin(), totalSamplesInCluster.end(), 0);
        for(int i=0;i<k;i++){
            fill(sumDistanceInCluster[i], sumDistanceInCluster[i]+sampleDimension, 0);
        }
        
        for(int i=0;i<sampleSize;i++){
            auto s = samples->getData(i);
            totalSamplesInCluster[ tags[i] ]++;
            for(int j=0;j<sampleDimension;j++){
                sumDistanceInCluster[tags[i]][j] += s[j];
            }
        }
        
        // checking for convergence
        pdelta = delta;
        delta = 0;
        for(int i=0;i<k;i++){
            for(int j=0;j<sampleDimension;j++){
                auto val = sumDistanceInCluster[i][j] / totalSamplesInCluster[i];
                delta += abs(centers[i][j] - val);
                centers[i][j] = val;
            }
        }
        auto ddelta = abs(pdelta - delta);
        if(ddelta < 0.5 || counter++>maxCounter){
            notConverged=false;
            if(counter==maxCounter){
                cout << "Kmean not converged after " << maxCounter << "  iterations." << endl ;
            }else{
                cout << "KMean converged after " << counter << " iterations, delta= " << ddelta << endl ;
                
            }
        }
        costHistory.push_back(kmeanCostFunction());
    }
    
    cout << "Kmean distortion:" << endl ;
    for(int i=0;i<costHistory.size();i++){
        cout << costHistory[i] << "," ;
    }cout << endl ;

    //parameter size = k*d
    //objective size = uknown
}

void Clustering::kmeanInitialize(){
    //initializing centers
    for(int i=0;i<k;i++){
        centers.push_back( vector<double>(samples->getData(rand()%samples->getSize())));
    }
    
    tags = vector<int>(sampleSize, -1);
    pdelta = 0.0;
    delta = 0.0;
    notConverged = true;
    counter=0;
}

void Clustering::kmeanUpdateCenters(DState* q){
    // Set the position of each cluster to the mean of all data points belonging to that cluster
    vector<double*> sumDistanceInCluster = vector<double*>();
    for(int i=0;i<k;i++){
        sumDistanceInCluster.push_back(new double[sampleDimension]());
    }
    
    vector<int> totalSamplesInCluster = vector<int>(k, 0);
    //fill(totalSamplesInCluster.begin(), totalSamplesInCluster.end(), 0);
    //for(int i=0;i<k;i++){
    //    fill(sumDistanceInCluster[i], sumDistanceInCluster[i]+sampleDimension, 0);
    //}
    for(int i=0;i<sampleSize;i++){
        auto s = samples->getData(i);
        totalSamplesInCluster[ q->tags[i] ]++;
        for(int j=0;j<sampleDimension;j++){
            sumDistanceInCluster[q->tags[i]][j] += s[j];
        }
    }
    pdelta = delta;
    delta = 0;
    for(int i=0;i<k;i++){
        for(int j=0;j<sampleDimension;j++){
            auto val = sumDistanceInCluster[i][j] / totalSamplesInCluster[i];
            delta += abs(q->centers[i][j] - val);
            q->centers[i][j] = val;
        }
    }
}

void Clustering::kmeanCheckConvergence(){
    // checking for convergence
    vector<vector<vector<double> > > states;
    
    auto ddelta = abs(pdelta - delta);
    if(ddelta < 0.0001 || counter++>iterations){
        notConverged=false;
        if(counter==iterations){
            cout << "Kmean not converged after " << iterations << "  iterations." << endl ;
        }else{
            cout << "KMean converged after " << counter << " iterations, delta= " << ddelta << endl ;
            
        }
    }
}

vector<double> Clustering::kmeanClusterEnergy(){
    vector<double> energy(k, 0);
    for(int i=0;i<sampleSize;i++){
        auto d = abs(distance(samples->getData(i), centers[tags[i]]));
        energy[tags[i]] += d*d;
    }
    return energy;
}

//double Clustering::distance(int dim, double* src, double* dst)
// computes the distortion of samples at every iteration of kmean algorithm
double Clustering::kmeanCostFunction(){
    double j=0;
    auto energy = kmeanClusterEnergy();
    for(int i=0;i<energy.size();i++){
        j += energy[i];
    }
    j /= (1.0*sampleSize);
    return j;
}

void Clustering::kmeanClassic(){
    cout << "Executing standard kmean clustering algorithm" << endl ;
    DState* q = initialize();
    //while(notConverged){
    for(int i=0;i<10;i++){
        evaluate(q);
        kmeanUpdateCenters(q);
        //kmeanCheckConvergence();
        //costHistory.push_back(kmeanCostFunction());
    }
    //cout << "Kmean distortion:" ;
    //for(int i=0;i<costHistory.size();i++){
    //    cout << costHistory[i] << "," ;
    //}cout << endl ;
    centers=q->centers;
    tags = q->tags;

}

DState* Clustering::initialize(){
    vector<vector<double> > root;
    for(int i=0;i<k;i++){
        root.push_back( vector<double>(samples->getData(rand()%samples->getSize())));
    }
    DState* s = new DState(settings, sampleSize);
    s->setParameter(root);
    evaluate(s);
    cost(s);
    return s;
}


template <class T, class S, class C>
S& Container(priority_queue<T, S, C>& q) {
    struct Queue : private priority_queue<T, S, C> {
        static S& Container(priority_queue<T, S, C>& q) {
            return q.*&Queue::c;
        }
    };
    return Queue::Container(q);
}

void Clustering::insert(DState* new_elem){
    states.push_back(new_elem);
    int buffersize=10;
    if(pq.size()<buffersize){
        pq.push(new_elem);
    }else{
        auto min = pq.top();
        if(new_elem->cost >min->cost){
            pq.pop();
            pq.push(new_elem);
        }
    }
    buffer = Container(pq);
}

DState* Clustering::globalStep(){
    //return states[rand()%states.size()];
    return buffer[rand()%buffer.size()];
}

DState* Clustering::localStep(DState* qnear){
    DState* qnew = new DState(qnear);
    qnew->centers[rand()%k] = vector<double>(samples->getData(rand()%samples->getSize()));
    /*vector<double> newcenter = vector<double>(sampleDimension,0);
    for(int i=0;i<10;i++){
        int randomSampleIndex = rand()%samples->getSize();
        auto randomSample = samples->getData(randomSampleIndex);
        for(int j=0;j<sampleDimension;j++){
            newcenter[j] += randomSample[j];
        }
    }
    for(int i=0;i<sampleDimension;i++) newcenter[i] /= 10.0;
    qnew->centers[rand()%k] = newcenter;*/
    return qnew;
}

double Clustering::cost(DState* q){
    vector<double> energy(k, 0);
    for(int i=0;i<sampleSize;i++){
        auto d = abs(distance(samples->getData(i), q->centers[q->tags[i]]));
        energy[q->tags[i]] += d*d;
    }
    q->cost=energy;
    
    q->totalCost=0;
    for(int i=0;i<energy.size();i++){
        q->totalCost += energy[i];
    }
    q->totalCost /= (1.0*sampleSize);
    return q->totalCost;
}

void Clustering::evaluate(DState* q){
    // Attribute the closest cluster to each data point
    for(int i=0;i<sampleSize;i++){
        double min_distance=numeric_limits<double>::max();
        for(int j=0;j<k;j++){
            auto dist = distance(samples->getData(i), q->centers[j]);
            if(dist<min_distance){
                min_distance=dist;
                q->tags[i]=j;
            }
        }
    }
}

void Clustering::kmeanDuplex(){
    auto root = initialize();
    insert(root);
    for(int iter=0;iter<iterations;iter++){
        auto qprev = globalStep();
        auto qnew  = localStep(qprev);
        evaluate(qnew);
        auto energy = cost(qnew);
        cout << energy << endl ;
        insert(qnew);
    }
    
    
    //finding optimum state
    auto optState = 0;
    auto optCost = states[optState]->totalCost;
    for(int i=1;i<states.size();i++){
        if( states[i]->totalCost<optCost){
            optState=i;
            optCost=states[optState]->totalCost;
        }
    }
    cout << "Best result: " << optCost << endl ;
    centers=states[optState]->centers;
    tags = states[optState]->tags;
}

void Clustering::train(string alg){
    if(alg=="kmean"){
        kmean();
    }else if(alg=="kmean-classic"){
        kmeanClassic();
    }else if(alg=="kmean-duplex"){
        kmeanDuplex();
    }else{
        cout << "Uknown clustering algorithm " << alg << endl ;

    }
}
