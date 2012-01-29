//Mike Maxim
//Class for rapid move insertion and retrieval for generated move lists

#ifndef __MOVELIST_H__
#define __MOVELIST_H__

#include <globals.h>
#include <move.h>

#define MOVELIST_MAXMOVES		90

// Fast array based move list holder used during move generation
class MoveList {
public:

	MoveList();

	move_t get(int) const;
	void insert(move_t);
	bool exists(move_t);
	void join(const MoveList&);
	void clear();
	int size() const;
	void sort(int*);

private:

	move_t m_movelist[MOVELIST_MAXMOVES];
	int m_length;
};

#endif
