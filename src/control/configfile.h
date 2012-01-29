//Mike Maxim
//Module for loading configuration files

#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__

#include <globals.h>
#include <strtoken.h>
#include <bufferedreader.h>

class ConfigFile {
public:

	static ConfigFile* getInstance();

	bool loadFile(const string&);
	string getValue(const string&);

protected:

	ConfigFile();

private:

	static ConfigFile* m_instance;
	map<string, string> m_values;
};

#endif
