//Mike Maxim
//Helper class with global statics used for converting moves between formats
//Move is the internal format described in move.h

#ifndef __NOTATION_H__
#define __NOTATION_H__

#include <globals.h>
#include <move.h>
#include <board.h>

class Notation {
public:

	static string MoveToSAN(move_t, Board&);
	static move_t SANtoMove(const string&, Board&, bool& success);
	static string MoveToXB(move_t, Board&);
	static move_t XBtoMove(const string&,Board&);

	static string get_algebra(int);
	static string get_file(int);
	static int from_algebra(const string&);

private:
	
	static string clarify(int,int,int,Board&);
	static void get_setpieces(vector<int>&,bitboard);

};

#endif
