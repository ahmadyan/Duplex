# Duplex Optimization

[![CircleCI](https://circleci.com/gh/ahmadyan/Duplex.svg?style=svg)](https://circleci.com/gh/ahmadyan/Duplex)

[Duplex's website](http://adel.ac/duplex)

Duplex is a high-performance global optimization algorithm for nonconvex, nonlinear, and functional optimization problems. The following core capabilities are included:

* Duplex can find global optimum of nonlinear non-convex functions.
* Duplex implements variety of gradient-descent based optimizations internally, such as Momentum, AdaDelta, Adamax. Duplex can also optimize when the gradient information are not available.
* Duplex supports unsupervised learning algoriths for clustering. 
* If the gradient information are not available (for circuit optimization), duplex uses reinforcement learning to predict the landscape of energy function.
* Duplex supports Synopsys HSPICE for solving nonlinear systems.

The latest release and a complete manual may be found at the Duplex home page: http://adel.ac/duplex

## Dependencies

Duplex requires the following dependencies to be installed:

* [Eigen 3.3.0](http://eigen.tuxfamily.org) linear algebra library. Install eigen using 
		<code>brew install eigen</code>
* [Boost 1.51](http://www.boost.org) Duplex requires both headers and compiled binaries. Install using <code>brew install boost</code>
* [Config4cpp](http://www.config4star.org/) configuration library. The binaries for the config4cpp for macOS are already shipped with Duplex (<code>/submodules/config4cpp/lib</code>).
* [Pangolin](https://github.com/stevenlovegrove/Pangolin) for plotting and managing display. The Pangolin will replace the gnuPlot. Duplex will install Pangolin as a submodule in <code>/submodules/Pangolin</code>.
* GnuPlot for drawing plots *(Optional)*. Install gnuplot using
	<code>brew install gnuplot</code>

## Build
Duplex uses [CMake](www.cmake.org) as a build system.

	git clone git://github.com/ahmadyan/Duplex
	cd Duplex
	mkdir build	
	cd build
	cmake ..
	make

Use <code>cmake .. -GXcode</code> to generate the xcode project file (or other generators).
Few examples are provided in <code>/bin</code> directory. Few MATLAB tests are available in the <code>/test</code> directory.

## Usage
* ./duplex --config example.cfg
* Currently the duplex binaries and the configurations has to be in the same directory, so please output the binary in the bin folder.
* Example configuration files are available in the /bin directory
* If using hspice as a numerical simulator, User should set an environment variable DUPLEX_SIM_ID to an integer value. If you don't want to set the environment variable, run Duplex from the python wrapper (test/duplex.py) in test directory. Python wrapper requires Python 3+.

## Misc
Duplex uses llvm coding style. Any PR will be formatted using clang-format.
