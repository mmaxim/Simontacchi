//Mike Maxim
//Definition of the basic opening book structures which currently define the file format
//Provides mechanism for the engine to look up book moves.

#ifndef __BOOK_H__
#define __BOOK_H__

#include <globals.h>
#include <board.h>
#include <notation.h>
#include <bufferedreader.h>

#define BOOK_MAGICNUM		0xBACCEF00
#define BOOK_ENDPOS			26

class BKMoveRecord {
public:

	BKMoveRecord() : m_freq(0) { }
	BKMoveRecord(move_t m, int freq) : m_move(m), m_freq(freq) { }

	string toString();

	move_t m_move;
	int m_freq;
};

class BKBoardRecord {
public:

	BKBoardRecord() : m_total(0) { }

	bool operator==(const BKBoardRecord& b) { return (m_board == b.m_board); } 
	bool operator<(const BKBoardRecord& b) { return (m_board < b.m_board); }

	void addMove(move_t);
	string toString();
	string toThinkString(Board&);

	bitboard m_board;
	vector<BKMoveRecord> m_moves;
	int m_total;
		
};

typedef map<bitboard, BKBoardRecord>	strboardmap;

class Book {
public:

	Book();

	bool open_book(const string&);
	void close_book();
	bool book_move(Board&,move_t&);
	bool get_bookmoves(Board&,BKBoardRecord&);
	int get_positions();

private:

	void proc_position(bitboard, FILE*);
	move_t get_probmove(const BKBoardRecord&);

	strboardmap m_book;
};

#endif
