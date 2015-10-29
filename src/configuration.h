#pragma once

#include <string>
#include <vector>
using namespace std;

class SettingsException{
public:
	SettingsException(const char * str);
	SettingsException(const SettingsException & other);
	~SettingsException();
	const char* what() const; // Accessor
private:
	char* m_str;
	SettingsException();
	SettingsException operator=(const SettingsException &);
};

class Settings{
public:
	Settings();
	~Settings();
	void parse(const char * cfgSource, const char * scope = "")throw (SettingsException);
	bool check(const char* name, const char* value) const throw (SettingsException);
	const char * lookupString(const char * name) const throw (SettingsException);
	void lookupList(const char *name, const char **&array,int &arraySize) const throw (SettingsException);
	virtual int lookupInt(const char * name) const throw(SettingsException);
	virtual float lookupFloat(const char * name) const throw(SettingsException);
	virtual bool lookupBoolean(const char * name) const throw(SettingsException);
	virtual int lookupDurationMilliseconds(const char * name) const throw(SettingsException);
	virtual int lookupDurationSeconds(const char * name) const throw(SettingsException);
    vector<string> listVariables(const char* name, const char*) const throw (SettingsException);
    void test() const throw (SettingsException);
private:
	char* m_scope;
	void* m_cfg;
	Settings(const Settings &);
	Settings & operator=(const Settings &);
};