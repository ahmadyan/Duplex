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

namespace{
	const size_t ERROR_IN_COMMAND_LINE = 1;
	const size_t SUCCESS = 0;
	const size_t ERROR_UNHANDLED_EXCEPTION = 2;
}

int main(int argc, char** argv){
    Log* log = new Log(true);
	log->log("Duplex optimization tool.");
	Settings*  settings = new Settings();
	srand((unsigned int)time(0));
	log->log("settings created, procceeding to parsing arguments");
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
        log->setVerbose(verbose);

		log->log("Parsing config file complete.");
        System* system = new System(settings);
		Duplex* duplex = new Duplex(settings);				log->log("Duplex core created.");
        boost::property_tree::ptree ptree;  //used to load/save the data
        if(settings->check("mode", "load")){
            read_xml(settings->lookupString("savefile"), ptree);               // Load the XML file into the property tree.
            duplex->load(&ptree);
        }else{
			duplex->setSystem(system);						log->log("System set.");
			duplex->setObjective();							log->log("Objective set.");
			duplex->initialize();							log->log("Duplex initialization complete.");
            log->tick();
			duplex->optimize();
            log->tock("Duplex main optimizatio loop");
            //saving the results into an xml file
            duplex->save(&ptree);
            ofstream savefile(settings->lookupString("savefile"));
            write_xml(savefile, ptree);
            savefile.close();
        }
        
        if(settings->check("plot.enable", "true")){
            Graphics* graphic = new Graphics(settings->lookupString("plot.gnuplot"));
            graphic->execute(duplex->draw());
            graphic->saveToPdf(settings->lookupString("output"));
            delete graphic;
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
	
	cin.get();
	return SUCCESS;
}
