#pragma once

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

class Graphics{
    string gnuplot;
    FILE *gnuplotPipe;
public:
    Graphics(string);
    ~Graphics();
    void emptyPlot(string,double,double,double,double,double,double,int,int,string,string,string);
    void emptyPlot(string title, double xmin, double xmax, double ymin, double ymax);
    void waitForKey();
    void saveToPdf(string path);
    void execute(string str);
};