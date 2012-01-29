//Mike Maxim
//Exception

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <globals.h>

class Exception {
public:

	Exception();
	Exception(const string& str);

	string getMessage();

private:

	string m_msg;
};

#endif //__EXCEPTION_H__
