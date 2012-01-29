//Mike Maxim
//Interior positional recognizer

#ifndef __INTRECOGNIZER_H__
#define __INTRECOGNIZER_H__

#include <globals.h>
#include <board.h>

#define INTRECOG_UNKNOWN		0x1
#define INTRECOG_DRAW			0x2
#define INTRECOG_EGTB			0x4

#define INTRECOG_DRAW_REP		0x10
#define INTRECOG_DRAW_MAT		0x20
#define INTRECOG_DRAW_50		0x40

#define INTRECOG_ISUNKNOWN(x)	(((x) & INTRECOG_UNKNOWN) == INTRECOG_UNKNOWN)
#define INTRECOG_ISDRAW(x)		(((x) & INTRECOG_DRAW) == INTRECOG_DRAW)
#define INTRECOG_DRAWTYPE(x)	((x) & 0xf0)
#define INTRECOG_MAKEDRAW(x)	(INTRECOG_DRAW | (x))

// Repetition Table
#define REPTABLE_SIZE			500
#define REPTABLE_EMPTY			0
#define REPTABLE_FILLED			1
#define REPTABLE_DELETED		2

typedef struct tagHISTENTRY {

	Board m_board;
	move_t m_move;
	bool m_capture, m_pawnmove;
} HistEntry;

class InteriorRecognizer {
public:

	InteriorRecognizer(int);

	DWORD probe(Board&, int&);

	void reset();
	void push_game_position(Board&,move_t);
	void pop_game_position();

	void add_search_position(Board&);
	void remove_search_position(Board&);

private:

	void update_reptable(Board&);
	bool is_repeated(Board&);
	void clear_reptable();
	bool is_reptable_empty();
	void remove_reptable(Board&);

	deque<HistEntry> m_gamehist;
	int m_drawscore;
	vector<bitboard> m_reptable[REPTABLE_SIZE]; // Repetition checker table
};

#endif