//Mike Maxim
//Opening book generator class. Essentially just enumerates all positions from a PGN file 
//and creates a book out of it. Currently no support for win/loss ratios which probably sucks
//but it will be fixed eventually.

#ifndef __BOOKGEN_H__
#define __BOOKGEN_H__

#include <globals.h>
#include <board.h>
#include <strtoken.h>
#include <game.h>
#include <pgnloader.h>
#include <book.h>

class BookGenerator {
public:

	BookGenerator();
	
	void create_book(const string&,const string&,int);

private:

	void write_to_file_txt(const string&);
	void write_to_file_bin(const string&);
	int convert(char);

	PGNLoader m_pgnload;
	strboardmap m_bmap;
};


#endif
