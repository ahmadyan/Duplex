//  Duplex optimization tool
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "log.h"
#include "configuration.h"
#include "duplex.h"
#include "system.h"
#include "graphics.h"
#include "clustering.h"
#include "plotfactory.h"
#include <config4cpp/Configuration.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "nonconvexoptimizer.h"
#include "descentOptimizer.h"
#include "functionalOptimizer.h"
#include "systemOptimizer.h"

using namespace config4cpp;

#if (_MSC_VER == 1900)	//Visual studio 2015
// temporary code, visual studio 2015 refactored CRT libraries. Now, some codes (specifically config4cpp) 
// will produce link libraries because they cannot find _iob_func (used in _sprintf).
// one solution is to recompile them using visual studio 2015 with legacy_stdio_definitions.lib additional dependency. 
// this is a temporary fix.
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void){return _iob;}
#endif

namespace{
	const size_t ERROR_IN_COMMAND_LINE = 1;
	const size_t SUCCESS = 0;
	const size_t ERROR_UNHANDLED_EXCEPTION = 2;
}

enum class mode {load, duplex, fopt, opt, sa, clustering, system, invalid};
mode getMode(Settings* settings){
    auto m=mode::invalid;
    if (settings->check("mode", "load")){
        m=mode::load;
    }else if (settings->check("mode", "duplex")){
        m=mode::duplex;
    }else if (settings->check("mode", "fopt")){
        m=mode::fopt;
    }else if (settings->check("mode", "opt")){
        m=mode::opt;
    }else if (settings->check("mode", "simulated-annealing")){
        m=mode::sa;
    }else if (settings->check("mode", "clustering")){
        m=mode::clustering;
    }else if (settings->check("mode", "system")){
        m=mode::system;
    }
    return m;
}

int main(int argc, char** argv){
	Log log;
    log << "Duplex optimization tool." << endl ;
	Settings*  settings = new Settings();
	srand((unsigned int)time(0));
    log << "settings created, procceeding to parsing arguments" << endl ;
	try{
		namespace po = boost::program_options;
		po::options_description desc("Options");
		desc.add_options()
			("help", "Print help messages")
			("verbose,v", po::value<int>(), "print words with verbosity (0 or 1)")
			("config,c", po::value<string>(), "Specifies the configuration file")
			("like", "this");
		po::variables_map vm;
		try{
			po::store(po::parse_command_line(argc, argv, desc), vm); // can throw 
			// --help option
			if (vm.count("help")){		
				std::cout << "Duplex optimization engine. " << std::endl
					      << "Version 0.1    (C) 2015 Seyed Nematollah (Adel) Ahmadyan, All right reserved." << std::endl
					      << desc << std::endl;
				return SUCCESS;
			}
			po::notify(vm); // throws on error, so do after help in case there are any problems 
		} catch (po::error& e){
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			cin.get();
			return ERROR_IN_COMMAND_LINE;
		}
		cout << "Parsing arguments complete. Proceed to parsing config file ..." << endl;
		settings->parse(vm["config"].as<std::string>().c_str(), "Duplex");

        bool verbose=true;
		try{
            verbose = vm["verbose"].as<int>();
		}catch (exception& e){
            verbose = settings->lookupBoolean("verbose");
		}
        log << "Parsing config file complete." << endl ;
        
		Duplex* duplex = NULL;
		Clustering* clustering = NULL;
        boost::property_tree::ptree ptree;  //used to load/save the data
        
        //determining which algorithm to execute
        auto m = getMode(settings);
        
        log.tick();
        
        switch(m){
            // -----------------------------------------------------
            case mode::load:
                // Load the XML file into the property tree.
				duplex = new NonconvexOptimizer(settings);
                read_xml(settings->lookupString("savefile"), ptree);
                duplex->load(&ptree);
                break;
            // -----------------------------------------------------
            case mode::duplex:
                duplex = new NonconvexOptimizer(settings);
                duplex->train();
                break;
                
            // -----------------------------------------------------
            case mode::fopt:
                duplex = new FunctionalOptimizer(settings);
                duplex->train();
                break;
            
            // -----------------------------------------------------
            case mode::opt:
                duplex = new DescentOptimizer(settings);
                duplex->train();
                break;
            
            // -----------------------------------------------------
            case mode::sa:
				duplex = new NonconvexOptimizer(settings);
                //duplex->simulated_annealing();
                break;
            
            case mode::system:
                duplex = new SystemOptimizer(settings);
                duplex->train();
                break;
                
            // -----------------------------------------------------
            case mode::clustering:
                clustering = new Clustering(settings);
                clustering->train(settings->lookupString("clustering.mode"));
                break;
            
            // -----------------------------------------------------
            case mode::invalid:
                log << "Unknown optimization mode is selected.";
				return 0;
        }
        log.tock("Duplex main optimizatio loop");
        
        //saving the results into an xml file
        //duplex->save(&ptree);
        //ofstream savefile(settings->lookupString("savefile"));
        //write_xml(savefile, ptree);
        //savefile.close();
        
        // Plotting the results
        if(settings->check("plot.enable", "true")){
            PlotFactory* pf = new PlotFactory(settings, duplex, clustering);
			vector<string> plots = settings->listVariables("plot", "uid-plot");
			for (int i = 0; i < plots.size(); i++){
				Graphics* graphic = new Graphics(settings->lookupString("plot.gnuplot"));
                string plotStr = pf->getPlot(i);
                graphic->execute(plotStr);
				//graphic->execute(duplex->draw(i));
				//graphic->saveToPdf(settings->lookupString("output"));
				delete graphic;
			}
        }
        
        //clean-up
        if(clustering) delete clustering;
        if(duplex) delete duplex;
		if(settings) delete settings;
        
	}catch (SettingsException se){
		std::cerr << "CONFIG ERROR: " << se.what() << std::endl << std::endl;
		cin.get();
		return ERROR_IN_COMMAND_LINE;
	}catch (std::exception& e){
		std::cerr << "Unhandled Exception reached the top of main: "
			<< e.what() << ", application will now exit" << std::endl;
		cin.get();
		return ERROR_UNHANDLED_EXCEPTION;
	}
    
    log << "Shutting down" << endl ;
	cin.get();
	return SUCCESS;
}
