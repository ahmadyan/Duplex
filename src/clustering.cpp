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

Clustering::~Clustering(){}
double Clustering::distance(int dim, double* src, double* dst){
    double d=0;
    for(int i=0;i<dim;i++){
        d += (src[i]-dst[i])*(src[i]-dst[i]);
    }
    return sqrt(d);
}

void Clustering::kmean(){
    cout << "Executing standard kmean clustering algorithm" << endl ;
    sampleSize = samples->getSize();
    k = settings->lookupInt("clustering.clusters");
    sampleDimension = samples->getDimension();
    
    vector<double*> sumDistanceInCluster;
    vector<int> totalSamplesInCluster(k, 0);
    for(int i=0;i<k;i++){
        sumDistanceInCluster.push_back(new double[sampleDimension]());
    }
    //initializing centers
    for(int i=0;i<k;i++){
        double* c = new double[sampleDimension];
        //double* s = samples->getData(rand()%samples->getSize());
        double* s = samples->getData(i);
        copy(s, s+sampleDimension, c);    //memcpy(c, s, sizeof(double)*d);
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
                auto dist = distance(sampleDimension, samples->getData(i), centers[j]);
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
                sumDistanceInCluster[tags[i]][j] += s[i];
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
    }
    //parameter size = k*d
    //objective size = uknown
}



void Clustering::kmeanAssignClusters(){
    // Attribute the closest cluster to each data point
    for(int i=0;i<sampleSize;i++){
        double min_distance=numeric_limits<double>::max();
        for(int j=0;j<k;j++){
            auto dist = distance(sampleDimension, samples->getData(i), centers[j]);
            if(dist<min_distance){
                min_distance=dist;
                tags[i]=j;
            }
        }
    }
}

void Clustering::kmeanInitialize(){
    sumDistanceInCluster = vector<double*>();
    totalSamplesInCluster = vector<int>(k, 0);
    for(int i=0;i<k;i++){
        sumDistanceInCluster.push_back(new double[sampleDimension]());
    }
    //initializing centers
    for(int i=0;i<k;i++){
        double* c = new double[sampleDimension];
        //double* s = samples->getData(rand()%samples->getSize());
        double* s = samples->getData(i);
        copy(s, s+sampleDimension, c);    //memcpy(c, s, sizeof(double)*d);
        centers.push_back(c);
    }
    tags = vector<int>(sampleSize, -1);
    
    pdelta = 0.0;
    delta = 0.0;
    notConverged = true;
    counter=0;
    maxCounter=100;
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
            sumDistanceInCluster[tags[i]][j] += s[i];
        }
    }
}

void Clustering::kmeanCheckConvergence(){
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
}

void Clustering::kmeanClassic(){
    cout << "Executing standard kmean clustering algorithm" << endl ;
    kmeanInitialize();
    while(notConverged){
        kmeanAssignClusters();
        kmeanUpdateCenters();
        kmeanCheckConvergence();
    }
}

Data* Clustering::getData(){
    return samples;
}

vector<int> Clustering::getTags(){
    return tags;
}
