//Mike Maxim
//BufferedReader implementation

#include <bufferedreader.h>
#include <string.h>

BufferedReader::BufferedReader() : m_index(0), m_file(NULL) {

}

BufferedReader::~BufferedReader() {
	close();
}

bool BufferedReader::open(const string& filename) {
	
	//Attempt to open the file
	if (NULL == (m_file = fopen(filename.c_str(), "r")))
		return false;

	m_index = BR_BUFFER_SIZE; m_end = 0;
	return true;
}

void BufferedReader::close() {
	if (m_file != NULL) {
		fclose(m_file);
		m_file = NULL;
	}
}

string BufferedReader::readLine() {

	string line="";
	char c[2];

	if (!m_file)
		return line;

	c[1] = '\0';
	while (1) {
		//We have read all our buffered data, let us get more
		if (m_index >= m_end) {
			if (!hasMoreBytes())
				return line;

			m_end = (int) fread(m_buffer, sizeof(char), BR_BUFFER_SIZE, m_file);
			m_index = 0;
		}

		//Check for end of line
		c[0] = m_buffer[m_index++];
		if (c[0] == '\n')
			return line;
		else
			line += c;
	}
}

void BufferedReader::read(char* buffer, int size) {

	char* line;
	int i;
	char c[2];

	if (!m_file)
		return;

	c[1] = '\0';
	line = new char[size];
	for (i = 0; i < size; i++) {
		//We have read all our buffered data, let us get more
		if (m_index >= m_end) {
			if (!hasMoreBytes()) {
				delete [] line;
				return;
			}

			m_end = (int) fread(m_buffer, sizeof(char), BR_BUFFER_SIZE, m_file);
			m_index = 0;
		}

		//Append new data
		c[0] = m_buffer[m_index++];
		line[i] = c[0];
	}

	memcpy(buffer, line, size);
	delete [] line;
}

bool BufferedReader::hasMoreBytes() {
	if (!m_file) return false;
	return (bool) (!feof(m_file) || m_index < m_end);
}


