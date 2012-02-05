//Mike Maxim
//Rotated BitBoard implementation of move generation and feature extraction

#ifndef __BOARD_H__
#define __BOARD_H__

#include <globals.h>
#include <move.h>
#include <movelist.h>
#include <strtoken.h>

#ifdef WIN32
#pragma warning(disable: 4800)
#endif

//Piece Types definitions
#define	BB_WHITE			0
#define BB_BLACK			1
#define BB_PAWNS			0
#define BB_KNIGHT			1
#define BB_BISHOP			2
#define BB_ROOK				3
#define BB_QUEEN			4
#define BB_KING				5		
#define BB_DRAW				6
#define BB_PAWNS_R90		6
#define BB_ROOKQUEEN		7
#define BB_BISHOPQUEEN		8
#define BB_MAXPIECE			9

//Game stage
#define GAME_OPENING		0
#define GAME_MIDDLE			1
#define GAME_ENDING			2

//Castling types
#define CASTLE_SHORT		0
#define CASTLE_LONG			1

// Bitboard type
#ifdef WIN32
typedef unsigned __int64	      bitboard;
#else
typedef unsigned long long int	  bitboard;
#endif

//Zobrist seed uni
#define	BB_ZOBRISTSEED		125

// Common masks
#define BB_ROWMASK			((bitboard)0xff)
#define BB_3RDRANK			((bitboard)0xff000000)
#define BB_4THRANK			((bitboard)0xff00000000)
#define BB_RIGHTFILE		(0x8080808080808080)
#define BB_LEFTFILE			(0x0101010101010101)
#define BB_ZERO				((bitboard)0)
#define BB_ONE				((bitboard)1)
#define BB_ALL				((bitboard)0xffffffffffffffff)

//Directions
#define BB_NORELATION		-1
#define BB_UPLEFT			0
#define BB_UPRIGHT			1
#define BB_UP				2
#define BB_DOWN				3
#define BB_DOWNRIGHT		4
#define BB_DOWNLEFT			5
#define BB_LEFT				6
#define BB_RIGHT			7

// BB Tasks
#define CLEAR_BIT(a,b)		((bitboard)(a) & Board::m_clearmasks[(b)])
#define IS_SET(bb,pos)		(((bb) & Board::m_masks[(pos)]))
#define BB_NEXTBIT(bb)		(Board::bb_lastset((bb)))

//Rotated BBS
#define BB_ROTATE_0			0 //No rotation
#define BB_ROTATE_R90		1 //Right rotated by 90 degrees
#define BB_ROTATE_R45L		2 //Left rotate by 45
#define BB_ROTATE_R45R		3 //Right rotate by 45
#define BB_MAXROTATES		4

// Board position ident
class BoardPosition {
public:
	BoardPosition() : m_color(BB_WHITE), m_type(BB_PAWNS) { }
	BoardPosition(int color, int type) : m_color(color), m_type(type) { }
	int m_color,m_type;
};

class Board {
public:

	Board();
	Board(const Board&);
	Board& operator=(const Board&);

	// Game modifiers
	void newGame();
	bool move(move_t);
	int turn() const;
	int otherTurn() const;
	int winner();
	bool stalemate();
	bool check();
	bool operator==(const Board&);
	void toggleTurn();

	//Feature Extraction
	int get_gamestage() const;
	int get_material(int) const;
	int get_piecetable(int) const;
	int get_nummoves() const;
	void get_square(int,BoardPosition&) const;
	int num_castles(int);
	bool is_castled(int);
	bitboard get_bitboard(int,int);
	bitboard get_column_bitboard(bitboard,int);
	bitboard get_pawn_column(int,int);
	int mat_gain(move_t) const;

	//Attacks
	bitboard get_all_attacks();
	bitboard get_single_pawn_attacks(int);
	bitboard get_single_pawn_attacks_own(int);
	bitboard get_pawn_attacks(); // NOT en passant
	bitboard get_rook_attacks(int);
	bitboard get_rook_attacks_own(int);
	bitboard get_bishop_attacks(int);
	bitboard get_bishop_attacks_own(int);
	bitboard get_knight_attacks(int);
	bitboard get_knight_attacks_own(int);
	bitboard get_king_attacks(int);
	bitboard get_king_attacks_own(int);
	int get_kingpos(int);
	bitboard get_signature();

	//FEN support
	bool fromFEN(const string&);
	string toFEN(bool moves = true) const;

	//Move Generation
	static void static_initialization();
	void generate_moves(MoveList&);
	void generate_non_capture_moves(MoveList&);
	void generate_capture_moves(MoveList&);
	void generate_check_filter(MoveList&,const MoveList&);
	bool validate_move(move_t);
    bool is_legal_move(move_t);
        
	//Debug
	void verify_rotator(move_t);
	void showbitboard(bitboard) const;
	string toString() const;

	unsigned int hash();
	unsigned int pawn_hash();

protected:

	friend class Notation;
	friend class Evaluator;
	friend class Player;
	friend class InteriorRecognizer;

	//Game Helpers
	void init();
	bool in_check();
	void clear_position(int,int);
	bool is_occupied(int) const;
	void finalizeSetup();

	// Generator helpers
	void generate_pawn_moves(MoveList&, bitboard opp, bool noncap = true);
	void generate_knight_moves(MoveList&, bitboard opp = BB_ALL);
	void generate_king_moves(MoveList&, bitboard opp, bool noncap = true);
	void generate_castle_moves(MoveList&,int);
	void generate_rook_moves(MoveList&, bool, bitboard opp = BB_ALL);
	void generate_bishop_moves(MoveList&, bool, bitboard opp = BB_ALL);
	void interp_pawn_bitboard(MoveList&,bitboard,int);
	bitboard rotate_bitboard(bitboard,int);

	//SEE helpers
	bitboard get_all_attacks_to(int);
	bitboard get_rank_attacks_own(int);
	bitboard get_file_attacks_own(int);
	bitboard get_diagA1_attacks_own(int);
	bitboard get_diagH8_attacks_own(int);

	// FEN Helpers
	void fen_parsepieces(const string&);
	string fen_buildpieces() const;

	//Aux Functions
	void copy(const Board&);
	void zobrist(bitboard&,bitboard&) const;

	// Game State
	bitboard m_allboard[BB_MAXROTATES],m_allpieces[2],m_zkey,m_pzkey;
	bitboard m_pbbs[BB_MAXPIECE][2];
	bool m_castle[2][2],m_hascastled[2];
	int m_turn,m_enpawn,m_nummoves,m_kingpos[2];
	int m_material[2],m_pt[2];

	// ***STATIC STATE***
	// Initialization helpers
	static void init_knight_attacks();
	static void init_rook_attacks();
	static void init_rank_attacks();
	static void init_file_attacks();
	static void init_bishop_attacks();
	static void init_king_attacks();
	static void init_diag_A1H8();
	static void init_diag_A8H1();
	static void init_zobrist();
	static void init_tropism();
	static void init_directions();

	//Aux Bitboard functions
	static bitboard get_mask(int);
	static bitboard get_safemask(int);
	static int bb_firstset(bitboard);
	static int bb_lastset(bitboard);
	static int bb_count(bitboard);
	static bitboard randbitboard();

	// Static move generation tables
	static bitboard m_knightatx[64],m_rankatx[64][256],m_fileatx[64][256],m_diagatxA1H8[64][256],m_diagatxA8H1[64][256];
	static bitboard m_files[8],m_dirbb[64][8];
	static int m_R90map[64],m_R45Lmap[64],m_R45Rmap[64],m_A1H8length[64],m_A8H1length[64];
	static int m_R45Lshift[64],m_R45Rshift[64],m_A1H8diagpos[64],m_A8H1diagpos[64],m_directions[64][64];
	static bitboard m_kingatx[64];
	static bitboard m_masks[64],m_clearmasks[64],m_vrankmasks[8];
	static move_t m_proms[4];
	static bitboard m_zobrist[6][2][64],m_zobristturn[2],m_zobristcastle[2][2],m_zobristenpawn[65];

};

#endif
