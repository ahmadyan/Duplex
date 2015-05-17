//
//  Configuration.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 5/5/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//  This class uses boos property tree to parse xml file and lexical cast

#include "configuration.h"
#include <boost/lexical_cast.hpp>

Configuration::Configuration(string filename){
	property_tree::read_xml(filename, db);
}

Configuration::~Configuration(){}

string Configuration::get(string key){
    return db.get<string>( key, "Key Not Found");
}

template<typename T> void Configuration::getParameter(string key, T* value){
    try{
        *value = boost::lexical_cast<T>(get(key));
    }catch (const std::exception&){
        cout << "[error] cast error in configuration " << endl ;
        *value = 0;
    }
}

//searches for all the occurance of key.name.label and returns them as a vector
//warning: this operation is relatively expensive and should be used as sparsely as possible
template<typename T> vector<T> Configuration::getVector(string key, string name, string label){
    vector<T> result;
    BOOST_FOREACH( boost::property_tree::ptree::value_type const& node, db.get_child( key )){
        boost::property_tree::ptree subtree = node.second;
        if( node.first == name ){
            BOOST_FOREACH( boost::property_tree::ptree::value_type const& v, subtree.get_child( "" ) ){
                if ( v.first == label ){
                    result.push_back(boost::lexical_cast<T>(subtree.get<std::string>(label)));
                }
            }
        }
    }
    return result;
}

//saves the modified database into an xml file
void Configuration::save(string filename){
    boost::property_tree::xml_writer_settings<char> w( ' ', 2 );
    write_xml( filename, db, std::locale(), w );
}
