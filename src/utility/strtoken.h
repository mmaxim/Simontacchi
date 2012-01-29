//Mike Maxim
//Java style StringTokenizer

#ifndef __STRTOKEN_H__
#define __STRTOKEN_H__

#include <globals.h>

class StringTokenizer : public vector<string> {
public:

	StringTokenizer(const string& rStr, const string& rDelimiters = " ");

};

#endif
