//Mike Maxim
//Crafty Display Manager

#ifndef __CRAFTYDISPLAYMAN_H__
#define __CRAFTYDISPLAYMAN_H__

#include <displayman.h>
#include <player.h>

class CraftyDisplayManager : public DisplayManager {
public:

	virtual void output_startsearch();
	virtual void output_excitement(int depth, int elapsed, Board& board, const Variation& var); 
	virtual void output_moveconsider(int side, int depth, int elapsed, int movenumber, int movetotal, int nodes, move_t move, Board& board);
	virtual void output_pv(int depth, int score, int elapsed, int nodes, Board& board, const Variation& var,bool);

	virtual void output_stats(const DisplayStats&);

protected:

	friend class DisplayManager;

	CraftyDisplayManager();

private:

	void clear_line();
};

#endif
