//Mike Maxim
//UCI Display Manager

#ifndef __UCIDISPLAYMAN_H__
#define __UCIDISPLAYMAN_H__

#include <displayman.h>

class UCIDisplayManager : public DisplayManager {
public:

	virtual void output_startsearch();
	virtual void output_excitement(int depth, int elapsed, Board&, const Variation&); 
	virtual void output_moveconsider(int side, int depth, int elapsed, int movenumber, int movetotal, int nodes, move_t move, Board& board);
	virtual void output_pv(int depth, int score, int elapsed, int nodes, Board& board, const Variation& var,bool);
	virtual void output_nodeinfo(int depth, int nodes, int elapsed);
	virtual void output_stats(const DisplayStats&);

protected:

	friend class DisplayManager;

	UCIDisplayManager();

};

#endif
