#include "plotFactory.h"

PlotFactory::PlotFactory(Settings* s, Stat* _stat, Duplex* _duplex, Clustering* _clustering){
    settings=s;
    stat=_stat;
    duplex=_duplex;
    clustering=_clustering;
}

PlotFactory::~PlotFactory(){
}

double minimum(double a, double b, double min){
    double m = min;
    if (a<m) m = a;
    if (b<m) m = b;
    return m;
}

double maximum(double a, double b, double max){
    double m = max;
    if (a>m) m = a;
    if (b>m) m = b;
    return m;
}

string PlotFactory::drawCanvas(string function, double xmin, double xmax, double ymin, double ymax, string plotType, string title){
    stringstream canvas;
    if(plotType=="default"){
        canvas << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0 title '" << title << "' with linespoints lt \"white\" pt 0.01\n";
    }else{
        canvas
        << "reset\n"
        
        << "f(x,y)="<< function << "\n"
        << "set xrange [" << xmin << ":" << xmax <<"]\n"
        << "set yrange [" << ymin << ":" << ymax <<"]\n"
        << "set isosample 250, 250\n"
        << "set table 'test.dat'\n"
        << "splot f(x,y)\n"
        << "unset table\n"
        
        << "set contour base\n"
        << "set cntrparam level incremental -3, 0.5, 3\n"
        << "unset surface\n"
        << "set table 'cont.dat'\n"
        << "splot f(x,y)\n"
        << "unset table\n"
        
        << "reset\n"
        << "set xrange [" << xmin << ":" << xmax <<"]\n"
        << "set yrange [" << ymin << ":" << ymax <<"]\n"
        << "unset key\n"
        << "set palette rgbformulae 33,13,10\n";
        
        if(plotType=="contour"){
            canvas << "p 'test.dat' with image, 'cont.dat' with linespoints lt \"white\" pt 0.01\n";
        } //the actual default is the surf plot
    }
    return canvas.str();
}

string PlotFactory::drawParameterTree(int x, int y, int z, string sizingPreference, string plotType, string title){
    auto db = duplex->getDB();
    string color = "blue";
    stringstream cmdstr;
    double xmin = 99, xmax = -99, ymin = 99, ymax = -99;
    for (int i = 1; i<db->getSize(); i++){   // i starts at 1 because we ignore the root
        State* s = db->getState(i);
        State* p = db->getState(s->getParentID());
        double iFromX = p->getParameter()[x];
        double iFromY = p->getParameter()[y];
        double iToX = s->getParameter()[x];
        double iToY = s->getParameter()[y];
        auto iFromZ = p->getObjective()[0];
        auto iToZ = s->getObjective()[0];
        
        xmin = minimum(iFromX, iToX, xmin);
        ymin = minimum(iFromY, iToY, ymin);
        xmax = maximum(iFromX, iToX, xmax);
        ymax = maximum(iFromY, iToY, ymax);
        
        cmdstr << " set arrow from " << iFromX << "," << iFromY << "," << iFromZ << " to " << iToX << "," << iToY << "," << iToZ << " nohead  lc rgb \"" << color << "\" lw 2 front \n";
    }
    
    if(sizingPreference=="default"){
        xmin=-1;xmax=1;ymin=-1;ymax=1;
    }
    
    stringstream board;
    board << drawCanvas(duplex->getSystem()->getFunction(0), xmin, xmax, ymin, ymax, plotType, title);
    board << cmdstr.str();
    return board.str();
}

string Duplex::drawObjectiveTree(int x, int y, string title){
    string color = "red";
    stringstream cmdstr;
    
    double xmin = 99, xmax = -99, ymin = 99, ymax = -99;
    for (int i = 1; i<db->getSize(); i++){   // i starts at 1 because we ignore the root
        State* s = db->getState(i);
        State* p = db->getState(s->getParentID());
        double iFromX = p->getObjective()[x];
        double iFromY = p->getObjective()[y];
        double iToX = s->getObjective()[x];
        double iToY = s->getObjective()[y];
        xmin = minimum(iFromX, iToX, xmin);
        ymin = minimum(iFromY, iToY, ymin);
        xmax = maximum(iFromX, iToX, xmax);
        ymax = maximum(iFromY, iToY, ymax);
        cmdstr << " set arrow from " << iFromX << "," << iFromY << " to " << iToX << "," << iToY << " nohead  lc rgb \"" << color << "\" lw 2 \n";
    }
    
    stringstream board;
    board << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0  title '" << title << "' with linespoints lt \"white\" pt 0.01";
    cmdstr << board.str() << "\n " << cmdstr.str();
    return cmdstr.str();
}

string PlotFactory::drawTrace(int x, int y, string title){
    auto db = duplex->getDB();
    string color = "black";
    stringstream cmdstr;
    
    double xmin = 99, xmax = -99, ymin = 99, ymax = -99;
    State* s = db->getOptimum();
    cout << "optimum state = " << s->getID() << ", " << s->getObjective()[0] << " , " << s->getObjective()[1] << ", " << s->getObjective()[2] << endl ;
    
    //todo: clean this up
    int maxarea=-1;
    int index=-1;
    for(int i=0;i<db->getSize();i++){
        State* x = db->getState(i);
        if(x->getObjective()[0]<50 && x->getObjective()[1]<100){
            double z = x->getObjective()[2];
            if(z>maxarea){
                z=maxarea;
                index=i;
            }
        }
    }
    s = db->getState(index);
    cmdstr << " set arrow from " << 50 << "," << 0 << " to " << s->getParameter()[x] << "," << s->getParameter()[y] << " nohead  lc rgb \"" << color << "\" lw 2 \n";
    while (s->getID() != 0){
        State* p = db->getState(s->getParentID());
        double iFromX = p->getParameter()[x];
        double iFromY = p->getParameter()[y];
        double iToX = s->getParameter()[x];
        double iToY = s->getParameter()[y];
        xmin = minimum(iFromX, iToX, xmin);
        ymin = minimum(iFromY, iToY, ymin);
        xmax = maximum(iFromX, iToX, xmax);
        if(xmax<50) xmax=52;    //todo: needs cleanup
        ymax = maximum(iFromY, iToY, ymax);
        cmdstr << " set arrow from " << iFromX << "," << iFromY << " to " << iToX << "," << iToY << " nohead  lc rgb \"" << color << "\" lw 2 \n";
        s = p;
    }
    
    stringstream board;
    board << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax << "] 0  title '" << title << "' with linespoints lt \"white\" pt 0.01";
    cmdstr << board.str() << "\n " << cmdstr.str();
    return cmdstr.str();
}

string PlotFactory::plotError(){
    auto error = stat->getError();
    stringstream cmdstr;
    cmdstr << "plot [" << 0 << ":" << error.size() << "][" << 0 << ":" << 1.1*error[0] << "] 0 with linespoints lt \"white\" pt 0.01" << endl;
    for (int i = 1; i < error.size(); i++){
        cmdstr << " set arrow from " << i - 1 << "," << error[i - 1] << " to " << i << "," << error[i] << " nohead  lc rgb \"red\" lw 2 \n";
    }
    return cmdstr.str();
}

string PlotFactory::plotDistance(){
    auto currentDistance = stat->getCurrentDistance();
    stringstream cmdstr;
    cmdstr << "plot [" << 0 << ":" << currentDistance.size() << "][" << 0 << ":" << 2.0*currentDistance[0] << "] 0 with linespoints lt \"white\" pt 0.01" << endl;
    for (int i = 1; i < currentDistance.size(); i++){
        cmdstr << " set arrow from " << i - 1 << "," << currentDistance[i - 1] << " to " << i << "," << currentDistance[i] << " nohead  lc rgb \"red\" lw 2 \n";
    }
    return cmdstr.str();
}


string PlotFactory::scatterPlot(){
    auto samples = clustering->getData();
    auto tags = clustering->getTags();
    stringstream cmdstr;
    int k=3;
    cmdstr << "plot ";
    for(int i=0;i<k;i++){
        cmdstr << "'-' w p ls " << i+1;
        if(i!=k-1) cmdstr << ", ";
        
    }
    cmdstr << endl;
    auto d = samples->getDimension();
    auto m = samples->getSize();
    for(int i=0;i<k;i++){
        for(int j=0;j<m;j++){
            if(tags[j]==i){
                auto s = samples->getData(j);
                for(int k=0;k<d;k++){
                    cmdstr << s[k] << " ";
                }
                cmdstr << endl ;
                
            }
        }
        cmdstr << "e" << endl;
    }
    return cmdstr.str();
}

string PlotFactory::getPlot(){
    vector<string> plots = settings->listVariables("plot", "uid-plot");
    string type = settings->lookupString(("plot." + plots[plotID] + ".type").c_str());
    if (type == "error"){
        return plotError();
    }
    else if (type == "distance"){
        return plotDistance();
    }else if (type=="data"){
        return scatterPlot();;
    }else if (type == "tree.parameter"){
        auto x = settings->lookupInt(("plot." + plots[plotID] + ".x").c_str());
        auto y = settings->lookupInt(("plot." + plots[plotID] + ".y").c_str());
        auto z = settings->lookupInt(("plot." + plots[plotID] + ".z").c_str());
        auto plotPreference = settings->lookupString(("plot." + plots[plotID] + ".plot").c_str());
        auto sizingPreference = settings->lookupString(("plot." + plots[plotID] + ".size").c_str());
        return ""; //drawParameterTree(x, y, z, sizingPreference, plotPreference, "param");
        /*
        if((x>parameterDimension-1) || (y>parameterDimension-1) || (z>objectiveDimension-1)){
            cout << "error: tree.parameter plotting error, configuration file contains an invalid parameter x,y,z " << endl ;
            return "";
        }else{
         
        }*/
    }
    else if (type == "tree.objective"){
        int x = settings->lookupInt(("plot." + plots[plotID] + ".x").c_str());
        int y = settings->lookupInt(("plot." + plots[plotID] + ".y").c_str());
        return ""; // drawObjectiveTree(x, y, "objective");
    }
    else if (type == "trace"){
        int x = settings->lookupInt(("plot." + plots[plotID] + ".x").c_str());
        int y = settings->lookupInt(("plot." + plots[plotID] + ".y").c_str());
        return drawTrace(x, y, "trace");
    }else{
        return "";
    }
}
