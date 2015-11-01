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
	Settings*  settings = new Settings();
	srand((unsigned int)time(0));

	try{
		namespace po = boost::program_options;
		po::options_description desc("Options");
		desc.add_options()
			("help", "Print help messages")
			("verbose,v", po::value<int>(), "print words with verbosity")
			("config,c", po::value<string>(), "Specifies the configuration file")
			("like", "this");
		po::variables_map vm;
		try{
			po::store(po::parse_command_line(argc, argv, desc), vm); // can throw 
			// --help option
			if (vm.count("help")){		
				std::cout << "Duplex optimization engine. " << std::endl
					      << "Version 0.0.1  (C) 2015 Seyed Nematollah (Adel) Ahmadyan, All right reserved." << std::endl
					      << desc << std::endl;
				return SUCCESS;
			}
			po::notify(vm); // throws on error, so do after help in case there are any problems 
		} catch (po::error& e){
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			return ERROR_IN_COMMAND_LINE;
		}
		cout << vm["config"].as<std::string>() << endl;
		settings->parse(vm["config"].as<std::string>().c_str(), "Duplex");
		Graphics* graphic = new Graphics(gnuPlot);
		System* system = new System(settings);
		Duplex* duplex = new Duplex(settings);
		duplex->setSystem(system);
		duplex->setObjective();
		duplex->initialize();
		duplex->optimize();
		graphic->execute(duplex->draw());
		graphic->saveToPdf(settings->lookupString("output"));
        boost::property_tree::ptree ptree;
        duplex->save(&ptree);
        ofstream savefile(settings->lookupString("savefile"));
        write_xml(savefile, ptree);
        savefile.close();
		delete duplex;
		delete system;
		delete settings;
	
	}catch (SettingsException se){
		std::cerr << "CONFIG ERROR: " << se.what() << std::endl << std::endl;
		return ERROR_IN_COMMAND_LINE;
	}catch (std::exception& e){
		std::cerr << "Unhandled Exception reached the top of main: "
			<< e.what() << ", application will now exit" << std::endl;
		return ERROR_UNHANDLED_EXCEPTION;
	}
	cin.get();
	return SUCCESS;
}
