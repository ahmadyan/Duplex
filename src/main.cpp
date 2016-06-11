//
//  main.cpp
//  duplex
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
#include <config4cpp/Configuration.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

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
        System* system = new System(settings);
        Duplex* duplex = new Duplex(settings);				log << "Duplex core created." << endl ;
        boost::property_tree::ptree ptree;  //used to load/save the data
        if(settings->check("mode", "load")){
            read_xml(settings->lookupString("savefile"), ptree);               // Load the XML file into the property tree.
            duplex->load(&ptree);
        }else{
            duplex->setSystem(system);						log << "System set." ;
            duplex->setObjective();							log << "Objective set. " ;
            duplex->initialize();							log << "Duplex initialization complete." << endl ;
            log.tick();

			if (settings->check("mode", "duplex")){
				duplex->optimize();
			}else if (settings->check("mode", "simulated-annealing")){
				duplex->simulated_annealing();
			}else if (settings->check("mode", "fopt")){
				duplex->functionalOptimization();
			}else if (settings->check("mode", "opt")){
				duplex->optimize();
			}else{
				log << "Unknown optimization mode is selected. Duplex currently supports: [load, duplex, simulated-annealing]";
			}
            log.tock("Duplex main optimizatio loop");
            //saving the results into an xml file
            duplex->save(&ptree);
            //ofstream savefile(settings->lookupString("savefile"));
            //write_xml(savefile, ptree);
            //savefile.close();
			//cout << "I'm here ..." << endl;
        }
        
        if(settings->check("plot.enable", "true")){
			vector<string> plots = settings->listVariables("plot", "uid-plot");
			for (int i = 0; i < plots.size(); i++){
				Graphics* graphic = new Graphics(settings->lookupString("plot.gnuplot"));
				graphic->execute(duplex->draw(i));
				//graphic->saveToPdf(settings->lookupString("output"));
				delete graphic;
			}
        }
        
        //clean-up
        delete duplex;
		delete system;
		delete settings;
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
