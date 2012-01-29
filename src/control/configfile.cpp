//Mike Maxim
//Configuration file manager

#include <configfile.h>

ConfigFile* ConfigFile::m_instance = NULL;

ConfigFile::ConfigFile() {

}

ConfigFile* ConfigFile::getInstance() {
    if (m_instance == NULL) {
        return (m_instance = new ConfigFile());
    }
    return m_instance;
}

bool ConfigFile::loadFile(const string& sfile) {

    BufferedReader reader;
    string line,key,value;

    if (!reader.open(sfile))
        return false;
    
    while (reader.hasMoreBytes()) {
        line = reader.readLine();

        //Comments
        if (line[0] == '#' || line.length() == 0)
            continue;

        StringTokenizer tokens(line,"=");
        
        if (tokens.size() == 0)
            return false;
        key = tokens[0];
        
        if (tokens.size() == 1)
            return false;
        value = tokens[1];

        m_values[key] = value;
    }

    return true;
}

string ConfigFile::getValue(const string& key) {
    map<string,string>::iterator i;
    if (m_values.end() == (i = m_values.find(key)))
        return "unknown";

    return m_values[key];
}
