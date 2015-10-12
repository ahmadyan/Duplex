#pragma once

#include <iostream>
#include <string>
#include <sstream>


using namespace std;

#ifdef _WIN32
    const string gnuPlot = "C:\\opt\\gnuplot\\bin\\gnuplot.exe -persist" ;
#elif  __linux__
    const string gnuPlot = "/usr/bin/gnuplot" ;
#elif  __APPLE__
    //const string gnuPlot = "/Applications/Gnuplot.app/Contents/Resources/bin/gnuplot" ; //Old gnuplot path (v4.0) on my Mac, does not support drawing a polygin, on aqua.
    //const string gnuPlot = "/usr/local/Cellar/gnuplot/5.0.0/bin/gnuplot"; //gnuplot if installed by brew
    const string gnuPlot = "/usr/local/bin/gnuplot"; //default gnuplot location of configured and built seperately
#else
    const string gnuPlot = "/usr/local/bin/gnuplot";
#error Undefined operating system/Unknown gnuplot path
#endif


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