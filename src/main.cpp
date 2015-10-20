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

namespace{
	const size_t ERROR_IN_COMMAND_LINE = 1;
	const size_t SUCCESS = 0;
	const size_t ERROR_UNHANDLED_EXCEPTION = 2;
}

int main(int argc, char** argv){
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
		Configuration* config = new Configuration(vm["config"].as<std::string>());
		Graphics* graphic = new Graphics(gnuPlot);

		System* system = new System(config);
		Duplex* duplex = new Duplex(config);

		double* init = new double[2]; init[0] = 0; init[1] = 0;
		double* goal = new double[2]; goal[0] = 2; goal[1] = 1;

		duplex->setSystem(system);
		duplex->initialize(init);
		duplex->setObjective(goal);

		duplex->optimize();
		graphic->execute(duplex->draw());
		graphic->saveToPdf("output.png");
		delete duplex;
		delete system;
		delete config;
		cin.get();
		return 0;

	}
	catch (std::exception& e){
		std::cerr << "Unhandled Exception reached the top of main: "
			<< e.what() << ", application will now exit" << std::endl;
		return ERROR_UNHANDLED_EXCEPTION;

	}

	
}
