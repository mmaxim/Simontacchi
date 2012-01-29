//Mike Maxim
//Static Evaluator based heavily on material. Positional analysis performed
//by piece tables and various other heuristics

#ifndef __EVALUATOR_H__
#define __EVALUATOR_H__

#include <evaluator.h>
#include <board.h>

//Material values
#define EVAL_SCORE_PAWN					100
#define EVAL_SCORE_KNIGHT				300
#define EVAL_SCORE_BISHOP				300
#define EVAL_SCORE_ROOK					500
#define EVAL_SCORE_QUEEN				900
#define EVAL_SCORE_KING					(EVAL_SCORE_QUEEN*20)
#define EVAL_PCVAL(x)					(Evaluator::m_pvals[(x)])

//Bounds
#define EVAL_MAX_POSITIONAL_CENT		95
#define EVAL_MAX_POSITIONAL_NORM		0.95

//Tropism
#define EVAL_KNIGHT_TROPISM(pos,kpos)	(7-((ABS(RANK((pos))-RANK((kpos))))-(ABS(FILE((pos))-FILE((kpos))))))
#define EVAL_ROOK_TROPISM(pos,kpos)		(7-(MAX(ABS(RANK((pos))-RANK((kpos))),2*ABS(FILE((pos))-FILE((kpos))))))
#define EVAL_QUEEN_TROPISM(pos,kpos)	(7-(MAX(ABS(RANK((pos))-RANK((kpos))),ABS(FILE((pos))-FILE((kpos))))))

// Entry for a pawn formation and its score
class PawnHashEntry {
public:

	PawnHashEntry() : m_turn(BB_WHITE), m_score(0) { m_pawns[0] = m_pawns[1] = BB_ALL; }
	PawnHashEntry(int score, int ptscore, int pturn, bitboard turn, bitboard oturn) 
		: m_turn(pturn), m_score(score), m_ptscore(ptscore) {
		m_pawns[0] = turn; m_pawns[1] = oturn;
	}

	bitboard m_pawns[2];
	int m_turn;
	int m_score,m_ptscore;
};

//Entry for the evaluation cache
class EvalCacheEntry {
public:

	EvalCacheEntry() : m_score(0), m_board(BB_ZERO) { }
	EvalCacheEntry(int score, bitboard board) : m_score(score), m_board(board) { }

	int m_score,m_stage;
	bool m_hascastled[2];
	bitboard m_board;
};

//Eval stats
class EvalStats {
public:

	EvalStats() : m_pawntt(0), m_evaltt(0), m_evals(0) { }

	int m_pawntt,m_evaltt;
	int m_evals;
};

//Square atx desc
typedef struct tagSQRATX {
	unsigned m_numatx : 3;
	unsigned m_pawn : 1;
	unsigned m_bishknight : 1;
	unsigned m_rook : 1;
	unsigned m_queen : 1;
	unsigned m_king : 1;
} sqr_atx;

//Main evaluator
class Evaluator {
public:

	Evaluator();
	Evaluator(bool,int);

	static void static_initialization();

	static void set_pawnsize(int);
	static void set_evalsize(int);

	int material(Board&);
	int eval(Board&,int,int);
	int fasteval(Board&);

	EvalStats& get_evalstats();
	void reset_evalstats();
	void verbose();

	static int m_pvals[BB_MAXPIECE];

private:

	friend class Board;

	int opening_positional(Board&);
	int middle_positional(Board&);
	int end_positional(Board&);

	int castle(Board&);
	int loopeval(Board&,int&,int&,bool);
	int kingsafety(Board&);
	int pawns(Board&,int&);
	int queens(Board&);
	bool pawnslower(Board&,bitboard,int,bool);
	int mobrook(Board&,int,int);
	int mobbishop(Board&, int);
	int mobknight(Board&, int);
	int mobqueen(Board&, int);

	void verbosity(int,int,int,int,int,int,int,int);

	bool m_verbose,m_lazy;
	int m_pawntt,m_lazywindow;
	EvalStats m_evalstats;
	sqr_atx m_wb[64],m_bb[64];

	static void init_tropism();

	static PawnHashEntry* m_hashpawn;
	static EvalCacheEntry* m_evalcache;
	static int m_hashsize,m_evalcachesize;
	static int m_knightpos[64],m_bishoppos[64],m_wpawnpos[64],m_bpawnpos[64],m_endkingpos[64];
	static int m_tropism[64][64][BB_MAXPIECE-1];
};

#endif
