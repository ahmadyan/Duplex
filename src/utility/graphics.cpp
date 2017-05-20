#include "graphics.h"
#include <fstream>
using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__TOS_WIN__)
#include <conio.h> //for getch(), needed in wait_for_key()
#endif

Graphics::Graphics(string path) {
  gnuplot = path;
#ifdef _WIN32
  gnuplotPipe = _popen(gnuplot.c_str(), "w");
#else
  gnuplotPipe = popen(gnuplot.c_str(), "w");
#endif
  emptyPlot("", 0, 1, 0, 1);
}

Graphics::~Graphics() {}

void Graphics::emptyPlot(string title, double xmin, double xmax, double ymin,
                         double ymax, double zmin, double zmax, int angleX,
                         int angleY, string xlabel, string ylabel,
                         string zlabel) {
  stringstream cmdstr;
  cmdstr << "splot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax
         << "][" << zmin << ":" << zmax
         << "] 0 with linespoints lt \"white\" pt 0.01";
  cmdstr << " title \"" << title << "\"  \n";
  cmdstr << "set xlabel \"$" << xlabel << "$\" \n";
  cmdstr << "set ylabel \"$" << ylabel << "$\" \n";
  cmdstr << "set zlabel \"$" << zlabel << "$\" \n";

  fprintf(gnuplotPipe, cmdstr.str().c_str());
  fflush(gnuplotPipe);
}

void Graphics::emptyPlot(string title, double xmin, double xmax, double ymin,
                         double ymax) {
  stringstream cmdstr;
  cmdstr << "plot [" << xmin << ":" << xmax << "][" << ymin << ":" << ymax
         << "] 0 with linespoints lt \"white\" pt 0.01";
  cmdstr << " title \"" << title << "\"  \n";

  fprintf(gnuplotPipe, cmdstr.str().c_str());
  fflush(gnuplotPipe);
}

void Graphics::waitForKey() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__TOS_WIN__) // every keypress registered, also arrow keys
  cout << endl << "Press any key to continue..." << endl;

  // FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
  _getch();
#elif defined(unix) || defined(__unix) || defined(__unix__) ||                 \
    defined(__APPLE__)
  cout << endl << "Press ENTER to continue..." << endl;

  std::cin.clear();
  std::cin.ignore(std::cin.rdbuf()->in_avail());
  std::cin.get();
#endif
  return;
}

void Graphics::saveToPdf(string path) {
  stringstream cmdstr;
  cmdstr << " set terminal png size 400,300 enhanced font \"Helvetica,20\" \n";
  cmdstr << " set output '" << path << "' \n";
  cmdstr << " replot \n";
  fprintf(gnuplotPipe, cmdstr.str().c_str());
  fflush(gnuplotPipe);
}

void Graphics::execute(string str) {
  fprintf(gnuplotPipe, str.c_str());
  fflush(gnuplotPipe);
  string buffer = "replot\n";
  fprintf(gnuplotPipe, buffer.c_str());
  fflush(gnuplotPipe);
  /*string filename = "duplex.gnuplot";
  ofstream plotfile;
  plotfile.open(filename);
  plotfile << str;
  plotfile.close();
  system("gnuplot duplex.gnuplot -persist");*/
}
