Duplex Optimization
===================

Duplex is a heuristic optimization engine for nonlinear, dynamic and functional optimization.
The following core capabilities are included:
  * Duplex can optimize nonlinear non-convex functions.

Installation
------------
Duplex requires the following dependencies to compile:
  * Boost 1.51 (Headers and compiled binaries)
  * [Config4cpp](http://www.config4star.org/)
  * User has to set an environment variable DUPLEX_SIM_ID to an integer value. If you don't want to set the environment variable, run Duplex from the python wrapper (duplex.py) in test directory. Python wrapper requires Python 3+.
  * GnuPlot for drawing plots.

Usage
-----
build-directory for xcode is /Users/adel/code/Duplex/xcode/Duplex.xcode/build/Debug/Duplex.xcode
Run --> ./Duplex.xcode --config /Users/adel/code/Duplex/bin/runtime.cfg
Need to set the environment variable DUPLEX_SIM_ID



Todo list:
----------  
  * Use JIT compiler to evaluate inline expressions faster
  * add time-annotation and functional optimization
