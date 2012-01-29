//Mike Maxim
//Global functions

#include <globals.h>

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

void output_unbuffered(const char* str) {
#ifdef WIN32
	DWORD dWord;
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),str,(DWORD)strlen(str),&dWord,NULL);
#else
    write(STDOUT_FILENO, str, strlen(str));
#endif
}

void input_unbuffered(char* buffer, int size) {
#ifdef WIN32
	_read(0,buffer,size);
#else
    read(STDIN_FILENO, buffer, size);
#endif
}

double getMS() {
#ifdef WIN32
	return timeGetTime()*.001;
#else
    timeval tv;
    gettimeofday(&tv, NULL);
    return double(tv.tv_sec) + double(tv.tv_usec)/1000000.0; 
#endif
}

string dwtos(uint64_t n) {

	string s="";

	s += itos((n >> 32)&0xffffffff);
	s += itos(n & 0xffffffff);

	return s;
}

string btos(bool i) {
    char buffer[256];
    snprintf(buffer, 256, "%b", i);
	return string(buffer);
}

string itos(int i) {
    char buffer[256];
    snprintf(buffer, 256, "%d", i);
	return string(buffer);
}

string dtos(double d) {
    char buffer[256];
    snprintf(buffer, 256, "%f", d);
	return string(buffer);
}
