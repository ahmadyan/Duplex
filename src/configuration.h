#pragma once
#include <string>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

class Configuration{
        property_tree::ptree db;
public:
    Configuration(string filename);
    void save(string);
	~Configuration();
    string get(string key);
    template<typename T> void getParameter(string key, T* value);
    template<typename T> vector<T> getVector(string path, string key, string name);
};