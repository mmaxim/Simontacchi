//Mike Maxim
//C++ version of the Java BufferedReader class

#ifndef __BUFFEREDREADER_H__
#define __BUFFEREDREADER_H__

#include <string>
#include <cstdio>
#include <iostream>

using namespace std;

#define BR_BUFFER_SIZE		16384

class BufferedReader {
public:

	BufferedReader();
	virtual ~BufferedReader();

	bool open(const string&);
	void close();

	string readLine();
	void read(char*, int);
	bool hasMoreBytes();

private:

	char m_buffer[BR_BUFFER_SIZE];
	int m_index,m_end;
	FILE* m_file;
};

#endif