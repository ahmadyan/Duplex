#pragma once
#include <string>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

class Configuration{
        property_tree::ptree pointerTree;
public:
    Configuration(string filename);
	~Configuration();
};