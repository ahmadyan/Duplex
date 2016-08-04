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
        while(getline(source,line)){
            double* sample = new double[dimension]();
            tokenizer<char_separator<char>> tokens(line, sep);
            int i=0;
            for(auto it=tokens.begin(); it!=tokens.end(); it++, i++){
                sample[i] = lexical_cast<double>(*it) ;
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


Data::~Data(){
}

string Data::toString(double* s){
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

double* Data::getData(int i){
    return payload[i];
}

int Data::getDimension(){
    return dimension;
}


string Data::plotScatter(){
    stringstream cmdstr;
    cmdstr << "plot 'tmp' with points" << endl;
    return cmdstr.str();
}

