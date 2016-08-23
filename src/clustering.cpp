#include "clustering.h"
#include <cmath>
#include <sstream>
#include <fstream>

Clustering::Clustering(Settings* s){
    settings = s;
    samples = new Data(settings);
    sampleSize = samples->getSize();
    k = settings->lookupInt("clustering.clusters");
    sampleDimension = samples->getDimension();
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
    sumDistanceInCluster = vector<double*>();
    totalSamplesInCluster = vector<int>(k, 0);
    for(int i=0;i<k;i++){
        sumDistanceInCluster.push_back(new double[sampleDimension]());
    }
    
    //initializing centers
    for(int i=0;i<k;i++){
        centers.push_back( vector<double>(samples->getData(rand()%samples->getSize())));
    }
    
    tags = vector<int>(sampleSize, -1);
    pdelta = 0.0;
    delta = 0.0;
    notConverged = true;
    counter=0;
    maxCounter=100;
}

void Clustering::kmeanAssignClusters(){
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
}

void Clustering::kmeanUpdateCenters(){
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
    pdelta = delta;
    delta = 0;
    for(int i=0;i<k;i++){
        for(int j=0;j<sampleDimension;j++){
            auto val = sumDistanceInCluster[i][j] / totalSamplesInCluster[i];
            delta += abs(centers[i][j] - val);
            centers[i][j] = val;
        }
    }
}

void Clustering::kmeanCheckConvergence(){
    // checking for convergence
    auto ddelta = abs(pdelta - delta);
    if(ddelta < 0.0001 || counter++>maxCounter){
        notConverged=false;
        if(counter==maxCounter){
            cout << "Kmean not converged after " << maxCounter << "  iterations." << endl ;
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
    kmeanInitialize();
    while(notConverged){
        kmeanAssignClusters();
        kmeanUpdateCenters();
        kmeanCheckConvergence();
        costHistory.push_back(kmeanCostFunction());
    }
    cout << "Kmean distortion:" ;
    for(int i=0;i<costHistory.size();i++){
        cout << costHistory[i] << "," ;
    }cout << endl ;
}

void Clustering::train(string alg){
    if(alg=="kmean"){
        kmean();
    }else if(alg=="kmeanClassic"){
        kmeanClassic();
    }
}