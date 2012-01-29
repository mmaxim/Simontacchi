//Mike Maxim
//Global stuff

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <map>
#include <string>
#include <vector>
#include <deque>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <map>
#include <cmath>
#include <cstdio>
#include <stack>
#include <inttypes.h>
#include <cstring>

#ifdef WIN32
#include <windows.h>
#pragma warning (disable : 4800)
#endif

using namespace std;

#define NAMESTR			"Simontacchi"
#define CVERSION		"1.8-Decepticon"

#define ASSERT(x)		if (!(x)) cout << "WARNING: Assertion @ File: " << __FILE__ \
									   << " Line: " << __LINE__ << " failed!" << endl;

#define FILE(i)			((i)%8)
#define RANK(i)			((i)>>3)
#define ABSOLUTEC(i,j)	(((i)<<3)+(j))

#define MAX(x,y)		(((x)>(y))?(x):(y))
#define MIN(x,y)		(((x)<(y))?(x):(y))
#define ABS(x)			((x)>=0?(x):(-(x)))

#define WRITEPIPE(s)	output_unbuffered((s))
#define READPIPE(s,n)   input_unbuffered((s),(n))

#define S_INFINITY		10000000

//Begin Win32/Compiler specific
#ifdef WIN32
#define REGISTER_PASS	__fastcall
#define NO_PROLOG		__declspec(naked)
#define STACK_PASS		__cdecl
#else
#define REGISTER_PASS
#define NO_PROLOG
#define STACK_PASS
#endif

typedef pair<int,int>&	ituple;
#ifndef WIN32
typedef unsigned int	DWORD;
#endif

double getMS();
string itos(int);
string dtos(double);
string btos(bool);
string dwtos(uint64_t);

void input_unbuffered(char*,int);
void output_unbuffered(const char*);

#endif
