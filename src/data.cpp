#include "data.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
using namespace boost;

Data::Data(Settings* settings){
    dimension = settings->lookupInt("data.dimension");
    auto size = settings->lookupInt("data.size");
    sourceFileName = settings->lookupString("data.source");
    format=settings->lookupString("data.format");
    hasLabel = settings->check("data.has_label", "true");
    bias = settings->check("data.bias", "true");
    if(bias) dimension++;
    labelColumn=-1;
    if(hasLabel){
        labelColumn = settings->lookupInt("data.label_column");
        labels = vector<int>(size);
    }
    
    importData(sourceFileName, size);
}

void Data::importData(string filename, int size){
    char_separator<char> sep;
    if(format=="tab"){
        sep = char_separator<char>(" ");
    }else{
        sep = char_separator<char>(", ");
    }
    
    ifstream source(sourceFileName);
    string line;
    if(source.is_open()){
        int row=0;
        while(getline(source,line)){
            row++;
            vector<double> sample;
            if(bias) sample.push_back(1);
            tokenizer<char_separator<char>> tokens(line, sep);
            int i=0;
            for(auto it=tokens.begin(); it!=tokens.end(); it++, i++){
                if(hasLabel && i==labelColumn){
                    labels[row] = lexical_cast<int>(*it) ;
                }else{
                    sample.push_back(lexical_cast<double>(*it));
                }
            }
            payload.push_back(sample);
        }
        source.close();
    }
    cout << "Imported " << payload.size() << " samples." << endl ;
}

void Data::exportData(string filename){
    auto size = getSize();
    ofstream savefile;
    savefile.open (filename);
    for(auto i=0;i<size; i++){
        savefile << toString(i) << endl;
    }
    savefile.close();
}

void Data::shuffleData(){
    random_shuffle(payload.begin(), payload.end());
}

Data::~Data(){
}

string Data::toString(double* s){
    std::ostringstream str;
    for(int i=0;i<dimension;i++){
        str << s[i] << " ";
    }
    return str.str();
}

string Data::toString(vector<double> s){
    std::ostringstream str;
    for(int i=0;i<dimension;i++){
        str << s[i] << " ";
    }
    return str.str();
}

string Data::toString(int i){
    return toString(getData(i));
}

int Data::getSize(){
    return (int)(payload.size());
}

vector<double> Data::getData(int i){
    return payload[i];
}

int Data::getLabel(int i){
    if(i<0 || i>labels.size()){
        cout << "Cannot access the label, index out of range " << i << " labels.size()=" << labels.size() << endl ;
        return -1;
    }else{
        return labels[i];
    }
}

void Data::setLabel(int i, int v){
    if(i<0 || i>labels.size()){
        cout << "Cannot set the label, index out of range " << i << " labels.size()=" << labels.size() << endl ;
    }else{
        labels[i]=v;
    }
}

int Data::getDimension(){
    return dimension;
}

string Data::plotScatter(){
    stringstream cmdstr;
    cmdstr << "plot 'tmp' with points" << endl;
    return cmdstr.str();
}
