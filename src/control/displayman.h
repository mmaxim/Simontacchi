//Mike Maxim
//Abstract interface for display of engine information

#ifndef __DISPLAYMAN_H__
#define __DISPLAYMAN_H__

#include <globals.h>
#include <variation.h>
#include <board.h>
#include <notation.h>

#define DISPLAY_MANAGER_LEGACY		0
#define DISPLAY_MANAGER_CRAFTY		1
#define DISPLAY_MANAGER_UCI			2
#define DISPLAY_MANAGER_MAX			3

typedef struct tagDISPLAYSTATS {
	int m_nodes,m_qnodes,m_evals,m_etthits,m_nullcut,m_ttreplace,m_ttmove;
	int m_makes,m_tthits,m_ttcutoff,m_pthits,m_failhigh,m_faillow,m_draws,m_egtbhit,m_egtbprobe;
	double m_elapsed,m_ttdensity;
} DisplayStats;

class DisplayManager {
public:

	static DisplayManager* getInstance(int);

	virtual void output_startsearch()=0;
	virtual void output_excitement(int depth, int elapsed, Board& board, const Variation& var)=0; 
	virtual void output_moveconsider(int side, int depth, int elapsed, int movenumber, int nodes, int movetotal, move_t move, Board& board)=0;
	virtual void output_pv(int depth, int score, int elapsed, int nodes, Board& board, const Variation& var, bool real)=0;

	virtual void output_stats(const DisplayStats&)=0;

protected:
	
	static DisplayManager* m_instance[DISPLAY_MANAGER_MAX];
};

#endif
