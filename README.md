Duplex Optimization
===================

[Duplex's website](http://ahmadyan.github.io/Duplex/)

Duplex is a high-performance optimization algorithm for nonconvex, nonlinear, and functional optimization problems.
The following core capabilities are included:
* Duplex can optimize nonlinear non-convex functions.
* Duplex implements variety of gradient-descent based optimizations internally, such as Momentum, AdaDelta, Adamax, 
* Duplex supports unsupervised learning algoriths for clustering. 
* If the gradient information are not available (for circuit optimization), duplex uses reinforcement learning to predict the landscape of energy function.

Installation
------------
Duplex requires the following dependencies to compile:
  * Boost 1.51 (Headers and compiled binaries)
  * [Config4cpp](http://www.config4star.org/)
  * User has to set an environment variable DUPLEX_SIM_ID to an integer value. If you don't want to set the environment variable, run Duplex from the python wrapper (duplex.py) in test directory. Python wrapper requires Python 3+.
  * GnuPlot for drawing plots.

Usage
-----
    * build-directory for xcode is /Users/adel/code/Duplex/xcode/Duplex.xcode/build/Debug/Duplex.xcode
    * Run --> ./Duplex.xcode --config /Users/adel/code/Duplex/bin/runtime.cfg
    * Need to set the environment variable DUPLEX_SIM_ID
