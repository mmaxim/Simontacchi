//Mike Maxim
//NegaMax version of alpha beta with null move forward pruning
//Transposition tables for ordering and values
//WinBoard semi-compatible

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <globals.h>
#include <board.h>
#include <evaluator.h>
#include <notation.h>
#include <book.h>
#include <clock.h>
#include <variation.h>
#include <displayman.h>
#include <intrecognizer.h>

// Player options
#define PLAYER_SHOWTHINKING			0
#define PLAYER_TRANSSIZE			1
#define PLAYER_TRANSENABLED			2
#define PLAYER_USETIMING			3
#define PLAYER_NULLDEPTH			4
#define PLAYER_BOOKUSAGE			5
#define PLAYER_DISPLAY				6
#define PLAYER_IDSTART				7
#define PLAYER_CONTEMPT				8
#define PLAYER_PVSENABLED			9
#define PLAYER_PTRANSSIZE			10
#define PLAYER_EVALCACHESIZE		11
#define PLAYER_LAZYEVAL				12
#define PLAYER_LAZYWINDOW			13
#define PLAYER_ASPIRATION			14
#define PLAYER_ASP_WINDOW			15
#define PLAYER_EXTFUTILE			16
#define PLAYER_RAZOR				17
#define PLAYER_NULLENABLED			18
#define PLAYER_MAXOPTIONS			19

//Depths
#define PLAYER_FRONTIERDEPTH		1
#define PLAYER_PREFRONTIERDEPTH		2
#define PLAYER_RAZORDEPTH			3
#define PLAYER_MATEDEPTH			30

#define PLAYER_MATE_SCORE			32759

//Fractional extension support
#define PLAYER_PLY_WIDTH			8
#define PLAYER_PLY_FULL(x)			((x)<<3)
#define PLAYER_PLY_LOGICAL(x)		((x)>>3)

//Extension types
#define PLAYER_EXT_CHECK			0
#define PLAYER_EXT_SINGLE			1
#define PLAYER_EXT_7THRANK			2
#define PLAYER_EXT_THREAT			3
#define PLAYER_EXT_MAX				4

// Transposition table value types
#define TT_UPPER				0
#define TT_LOWER				1
#define TT_EXACT				2
#define PLAYER_TT_HARDHIT		0
#define PLAYER_TT_SOFTHIT		1
#define PLAYER_TT_MISS			2
#define PLAYER_LIN_DISTANCE		10

typedef struct tagPOSINFO {

	move_t m_move; //Best position move
	int m_val; //Value for the position
	char m_type, //Type of value
		 m_depth; //Depth position found at
	bool m_check, //Is this position in check?
		 m_nullmove; //Is a null move allowed here?

} StoredPosInfo;

// Entry in the main transposition table
typedef struct tagTTENTRY {
	bitboard m_sig; // Zobrist key
	StoredPosInfo m_info;
	bool m_pvroot;
} TTEntry;

// Entry in the killer table
typedef struct tagKILLERENTRY {

	move_t m_moves[2];
	int m_scores[2];

} KillerEntry;

typedef struct tagGAMETREE {
	int m_alpha,					// Lower Bound
		m_beta,						// Upper Bound
		m_depth,					// Think Depth
		m_capsqr,					// Last capture location, -1 if none
		m_cappiece,					// Piece who did the capture
		m_descent,					// How many rescursive calls have we made?
		m_exts[PLAYER_EXT_MAX];		// Check extension
	Variation m_pv;					// Principal Variation
	move_t m_lastmove;				// Last move made in PVS process
	bool m_nullmove,				// Can we make the null move?
		 m_earlyallowed,			// Allowed to early abort?
		 m_endgame,					// Are we in the endgame?
		 m_root,					// Are we at the root?
		 m_extended,				// Have we extended this branch?
		 m_iid;						// Can we do IID at this node?
} GameTree;

typedef struct tagPLAYERINIT {
	string m_bookfile;
	bool m_showthinking,m_transenabled,m_usetiming,m_usebook,m_pvs,m_lazy,m_asp,m_nullenabled;
	int m_transsize,m_nulldepth,m_display,m_idstart,m_contempt,m_ptranssize;
	int m_evalcachesize,m_lazywindow,m_abwindow,m_extfutile,m_razor;
} PlayerInit;

// The main AI class
class Player {
public:

	Player();
	virtual ~Player();

	// Init the player
	void init(const PlayerInit&);
	void clear_trans();

	// Move
	int get_move(Board&, move_t&, double);
	void move_now(); //Force move
	void modify_timeleft(double);

	// Interior Recognizer
	InteriorRecognizer* getRecognizer();

	// Reset (should be phased out)
	void reset();

	// Get stored info
	move_t get_hint();
	Book* get_book();

	// In move stats
	int get_rootnodes();
	int get_currentnode();
	move_t get_currentmove();
	int get_elapsed();
	int get_nodes();
	int get_ply();
	Variation* get_current_pv();

	void toggleOption(int,int);
	void show_config();
	void change_books(const string&);
	int getOption(int);

private:

	friend class Engine;
	friend class WinboardController;

	int m_options[PLAYER_MAXOPTIONS];

	//Search helpers
	int search(Board&,move_t&);
	int pvs(Board&,GameTree*);
	int quies(Board&,int,int,bool);
	void quies_order(Board&, MoveList&, MoveList&, const MoveList&);
	void move_order(Board&,MoveList&);
	int move_weight(Board&,move_t,bool&);
	void update_killers(move_t,int,int);
	void update_hh(move_t,int);

	//SEE
	int see(Board&,int,int,int);
	void see_peek(Board&,int,bitboard&,int,int,int);

	//Table helpers
	int trans_lookup(Board&,int,int,int,StoredPosInfo&);
	void trans_store(Board&,const StoredPosInfo&,bool);
	TTEntry* trans_getentry(Board&,bool);
	bool get_stored_info(Board&,GameTree*,StoredPosInfo&,MoveList&,bool);

	//Misc
	void gather_pv(Board&,Variation*,move_t fmove = MOVE_EMPTY);
	void output_thinking(Board&,Variation*,int,double);
	void output_excitement(Board&,int,move_t);
	int maxhh();
	bool timeup();

	Evaluator* m_eval; // The evaluator
	static Book* m_book; // The opening book

	int m_nodes,m_qnodes,m_tnodes,m_orders,m_curiddepth,m_research,m_tthits,m_ttcutoff,m_museq,m_failhigh;
	int m_oldscore,m_roots,m_curroot,m_faillow,m_nullcut,m_ttreplace,m_ttmove,m_draws,m_egtbhit,m_egtbprobe;
	double m_starttime,m_endtime,m_timeallowed,m_lastnode;
	bool m_earlyabort,m_movenow; // Did we time abort?
	Variation m_oldline; // Previous best pv
	move_t m_currootmove;

	int m_histheur[66][66][6]; //History heuristic
	KillerEntry m_killers[PLAYER_MATEDEPTH]; // The Killer table
	InteriorRecognizer* m_recog;

	// Transposition Tables
	static TTEntry* m_transtable; // Main table
};

#endif
