//
//  Configuration.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 5/5/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//  This class uses boos property tree to parse xml file and lexical cast
//  Complete overhaul on 10/21/15. Replaced my own configuration class with config4cpp library.

#include "configuration.h"
#include <config4cpp/Configuration.h>
//#include "FallbackConfiguration.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace config4cpp;
using namespace std;

#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
int strcpy_s(char *strDestination, const char *strSource){
    strcpy(strDestination, strSource);
    return 0;
}

int strcpy_s(char *strDestination, size_t numberOfElements, const char *strSource){
    return strcpy_s(strDestination, strSource);
}

int strcat_s(char *strDestination, const char *strSource){
    strcat(strDestination, strSource);
    return 0;
}
#endif


// Exception
SettingsException::SettingsException(const char * str){
	m_str = new char[strlen(str) + 1];
	strcpy_s(m_str, strlen(str) + 1, str);
}

SettingsException::SettingsException(const SettingsException & other){
	m_str = new char[strlen(other.m_str) + 1];
	strcpy_s(m_str, strlen(other.m_str) + 1, other.m_str);
}

SettingsException::~SettingsException(){
	delete[] m_str;
}

const char *SettingsException::what() const{
	return m_str;
}

// Settings

Settings::Settings(){
	m_cfg = Configuration::create();
	m_scope = 0;
}

Settings::~Settings(){
	delete[] m_scope;
	((Configuration *)m_cfg)->destroy();
}

void Settings::parse(const char *cfgSource,const char *	scope) throw (SettingsException){
	Configuration * cfg = (Configuration *)m_cfg;

	m_scope = new char[strlen(scope) + 1];
	strcpy_s(m_scope, strlen(scope) + 1, scope);
	try {
		if (cfgSource != 0 && strcmp(cfgSource, "") != 0) {
			cfg->parse(cfgSource);
		}
		//cfg->setFallbackConfiguration(Configuration::INPUT_STRING, FallbackConfiguration::getString());
	}
	catch (const ConfigurationException & ex) {
		throw SettingsException(ex.c_str());
	}
}

const char * Settings::lookupString(const char * name) const throw (SettingsException){
	Configuration * cfg = (Configuration *)m_cfg;
	try {
		return cfg->lookupString(m_scope, name);
	}catch (const ConfigurationException & ex) {
		throw SettingsException(ex.c_str());
	}
}

void Settings::lookupList(const char* name, const char** &array, int& arraySize) const throw (SettingsException){
	Configuration * cfg = (Configuration *)m_cfg;
	try {
		cfg->lookupList(m_scope, name, array, arraySize);
	}catch (const ConfigurationException & ex) {
		throw SettingsException(ex.c_str());
	}
}

int Settings::lookupInt(const char * name) const throw (SettingsException){
	Configuration * cfg = (Configuration *)m_cfg;
	try {
		return cfg->lookupInt(m_scope, name);
	} catch (const ConfigurationException & ex) {
		throw SettingsException(ex.c_str());
	}
}

float Settings::lookupFloat(const char * name) const throw (SettingsException){
	Configuration * cfg = (Configuration *)m_cfg;
	try {
		return cfg->lookupFloat(m_scope, name);
	} catch (const ConfigurationException & ex) {
		throw SettingsException(ex.c_str());
	}
}

bool Settings::lookupBoolean(const char * name) const throw (SettingsException){
	Configuration * cfg = (Configuration *)m_cfg;
	try {
		return cfg->lookupBoolean(m_scope, name);
	}catch (const ConfigurationException & ex) {
		throw SettingsException(ex.c_str());
	}
}

int Settings::lookupDurationMilliseconds(const char * name) const throw (SettingsException){
	Configuration * cfg = (Configuration *)m_cfg;
	try {
		return cfg->lookupDurationMilliseconds(m_scope, name);
	}catch (const ConfigurationException & ex) {
		throw SettingsException(ex.c_str());
	}
}

int Settings::lookupDurationSeconds(const char * name) const throw (SettingsException){
	Configuration * cfg = (Configuration *)m_cfg;
	try {
		return cfg->lookupDurationSeconds(m_scope, name);
	}catch (const ConfigurationException & ex) {
		throw SettingsException(ex.c_str());
	}
}

bool Settings::check(const char * name, const char* value) const throw (SettingsException){
	Configuration * cfg = (Configuration *)m_cfg;
	try {
		const char* result = cfg->lookupString(m_scope, name);
		if (strcmp(result, value) == 0) return true;
		else return false;
	}
	catch (const ConfigurationException & ex) {
		throw SettingsException(ex.c_str());
	}
}

// list the values for all the uid-elements matching the uid name. the scope is defined as m_scope.name
// for example, parameter.uid-parameter.name --> returns the names for all parameter.uid-parameter
vector<string> Settings::listValues(const char* name, const char* uid) const throw (SettingsException){
	Configuration * cfg = (Configuration *)m_cfg;
	vector<string> list;
	char scope[100];
	strcpy_s(scope, m_scope);
	strcat_s(scope, ".");
	strcat_s(scope, name);
	config4cpp::StringVector result;
	try {
		cfg->listLocallyScopedNames(m_scope, name, Configuration::CFG_SCOPE_AND_VARS, true, result);
		for (int i = 0; i<result.length(); i++){
			if (cfg->uidEquals(result[i], uid)){
				list.push_back(cfg->lookupString(scope, result[i]));
			}
		}
	}
	catch (const ConfigurationException & ex) {
		cout << ex.c_str() << endl;
		throw SettingsException(ex.c_str());
	}
	return list;
}

// list the names of all uids matching the uid in the m_scope.name scope
// for example, parameter.uid-parameter.name --> returns parameter.uid-000000000-parameter.name, parameter.uid-000000001-parameter.name, etc.
// note this doesn't return the actual name, instead it just says what the uid variable is listed as, should use lookup afterward.
vector<string> Settings::listVariables(const char* name, const char* sub) const throw (SettingsException){
    Configuration * cfg = (Configuration *)m_cfg;
    vector<string> list;
    config4cpp::StringVector result;
    try {
        cfg->listLocallyScopedNames(m_scope, name, Configuration::CFG_SCOPE_AND_VARS, true, result);
        for(int i=0;i<result.length();i++){
            if (cfg->uidEquals(result[i], sub))
                list.push_back(string(result[i]));
        }
    } catch (const ConfigurationException & ex) {
        throw SettingsException(ex.c_str());
    }
    return list;
}

void Settings::test() const throw (SettingsException){
    Configuration * cfg = (Configuration *)m_cfg;
    config4cpp::StringVector result;
    try {
        //cfg->listLocallyScopedNames(m_scope, "", Configuration::CFG_SCOPE_AND_VARS, true, result);
        cfg->listFullyScopedNames(m_scope, "", Configuration::CFG_SCOPE_AND_VARS, true, result);
        for(int i=0;i<result.length();i++){
            std::cout << result[i] << std::endl ;
        }
    } catch (const ConfigurationException & ex) {
        throw SettingsException(ex.c_str());
    }
}