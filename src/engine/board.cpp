//Mike Maxim
//Board implementation

#include <board.h>
#include <notation.h>
#include <evaluator.h>

bitboard Board::m_dirbb[64][8];
int Board::m_directions[64][64];
move_t Board::m_proms[4];

Board::Board() {

}

Board::Board(const Board& board) {
	copy(board);
}

Board& Board::operator =(const Board& board) {
	if (&board != this)
		copy(board);

	return *this;
}

bool Board::operator ==(const Board& board) {

	// Just make sure all status flags and bitboards are the same
	bool equal = ((m_allpieces[0] == board.m_allpieces[0]) && (m_allpieces[1] == board.m_allpieces[1]) &&
		    (m_pbbs[BB_ROOK][0] == board.m_pbbs[BB_ROOK][0]) && (m_pbbs[BB_ROOK][1] == board.m_pbbs[BB_ROOK][1]) &&
			(m_pbbs[BB_BISHOP][0] == board.m_pbbs[BB_BISHOP][0]) && (m_pbbs[BB_BISHOP][1] == board.m_pbbs[BB_BISHOP][1]) &&
			(m_pbbs[BB_KNIGHT][0] == board.m_pbbs[BB_KNIGHT][0]) && (m_pbbs[BB_KNIGHT][1] == board.m_pbbs[BB_KNIGHT][1]) &&
			(m_pbbs[BB_QUEEN][0] == board.m_pbbs[BB_QUEEN][0]) && (m_pbbs[BB_QUEEN][1] == board.m_pbbs[BB_QUEEN][1]) &&
			(m_pbbs[BB_KING][0] == board.m_pbbs[BB_KING][0]) && (m_pbbs[BB_KING][1] == board.m_pbbs[BB_KING][1]) &&
			(m_pbbs[BB_PAWNS][0] == board.m_pbbs[BB_PAWNS][0]) && (m_pbbs[BB_PAWNS][1] == board.m_pbbs[BB_PAWNS][1]) &&
			(m_turn == board.m_turn) && (m_enpawn == board.m_enpawn) &&
			(m_castle[0][0] == board.m_castle[0][0]) &&
			(m_castle[0][1] == board.m_castle[0][1]) && (m_castle[1][0] == board.m_castle[1][0]) &&
			(m_castle[1][1] == board.m_castle[1][1]));
	
	return (equal);
}

//Clear everything
void Board::init() {
	m_turn = BB_WHITE; m_enpawn=-1; m_nummoves = 0;
	m_castle[0][0] = m_castle[0][1] = m_castle[1][0] = m_castle[1][1] = true;
	m_hascastled[0] = m_hascastled[1] = false;
	m_zkey = BB_ZERO;
	m_allpieces[0] = m_allpieces[1]=BB_ZERO;
	m_allboard[0] = m_allboard[1]=BB_ZERO;
	m_pbbs[BB_PAWNS][0] = m_pbbs[BB_PAWNS][1]=BB_ZERO;
	m_pbbs[BB_KNIGHT][0] = m_pbbs[BB_KNIGHT][1]=BB_ZERO;
	m_pbbs[BB_BISHOP][0] = m_pbbs[BB_BISHOP][1]=BB_ZERO;
	m_pbbs[BB_QUEEN][0] = m_pbbs[BB_QUEEN][1]=BB_ZERO;
	m_pbbs[BB_ROOK][0] = m_pbbs[BB_ROOK][1]=BB_ZERO;
	m_pbbs[BB_KING][0] = m_pbbs[BB_KING][1]=BB_ZERO; m_kingpos[0] = m_kingpos[1] = 0;
}

//Copy a board, used for unmake()
void Board::copy(const Board& b) {
	int i;

	for (i = 0; i < BB_MAXROTATES; i++)
		m_allboard[i] = b.m_allboard[i];

	m_allpieces[0] = b.m_allpieces[0];
	m_allpieces[1] = b.m_allpieces[1];
	m_pbbs[BB_ROOK][0] = b.m_pbbs[BB_ROOK][0];
	m_pbbs[BB_ROOK][1] = b.m_pbbs[BB_ROOK][1];
	m_pbbs[BB_BISHOP][0] = b.m_pbbs[BB_BISHOP][0];
	m_pbbs[BB_BISHOP][1] = b.m_pbbs[BB_BISHOP][1];
	m_pbbs[BB_KNIGHT][0] = b.m_pbbs[BB_KNIGHT][0];
	m_pbbs[BB_KNIGHT][1] = b.m_pbbs[BB_KNIGHT][1];
	m_pbbs[BB_QUEEN][0] = b.m_pbbs[BB_QUEEN][0];
	m_pbbs[BB_QUEEN][1] = b.m_pbbs[BB_QUEEN][1];
	m_pbbs[BB_KING][0] = b.m_pbbs[BB_KING][0];
	m_pbbs[BB_KING][1] = b.m_pbbs[BB_KING][1];
	m_pbbs[BB_PAWNS][0] = b.m_pbbs[BB_PAWNS][0];
	m_pbbs[BB_PAWNS][1] = b.m_pbbs[BB_PAWNS][1];
	m_pbbs[BB_PAWNS_R90][0] = b.m_pbbs[BB_PAWNS_R90][0];
	m_pbbs[BB_PAWNS_R90][1] = b.m_pbbs[BB_PAWNS_R90][1];
	m_pbbs[BB_ROOKQUEEN][0] = b.m_pbbs[BB_ROOKQUEEN][0];
	m_pbbs[BB_ROOKQUEEN][1] = b.m_pbbs[BB_ROOKQUEEN][1];
	m_pbbs[BB_BISHOPQUEEN][0] = b.m_pbbs[BB_BISHOPQUEEN][0];
	m_pbbs[BB_BISHOPQUEEN][1] = b.m_pbbs[BB_BISHOPQUEEN][1];

	m_turn = b.m_turn; m_nummoves = b.m_nummoves;
	m_enpawn = b.m_enpawn;
	m_castle[0][0] = b.m_castle[0][0];
	m_castle[0][1] = b.m_castle[0][1];
	m_castle[1][0] = b.m_castle[1][0];
	m_castle[1][1] = b.m_castle[1][1];
	m_hascastled[0] = b.m_hascastled[0];
	m_hascastled[1] = b.m_hascastled[1];
	m_kingpos[0] = b.m_kingpos[0];
	m_kingpos[1] = b.m_kingpos[1];
	m_zkey = b.m_zkey; m_pzkey = b.m_pzkey;
	m_material[0] = b.m_material[0]; m_material[1] = b.m_material[1];
	m_pt[0] = b.m_pt[0]; m_pt[1] = b.m_pt[1];

}

//Init all board static data
void Board::static_initialization() {
	int i;

	// Make mask bitboards
	for (i = 0; i < 64; i++) {
		m_masks[i] = (bitboard)1 << i;
		m_clearmasks[i] = ~m_masks[i];
	}
	init_knight_attacks();
	init_rook_attacks();
	init_bishop_attacks();
	init_king_attacks();
	init_zobrist();
	init_directions();
	Evaluator::static_initialization();

	// Promotions
	MOVE_SETPROMOTION(m_proms[0],BB_KNIGHT);
	MOVE_SETPROMOTION(m_proms[1],BB_BISHOP);
	MOVE_SETPROMOTION(m_proms[2],BB_ROOK);
	MOVE_SETPROMOTION(m_proms[3],BB_QUEEN);

	// Init file masks
	m_files[0]=BB_LEFTFILE;
	m_files[1]=0x0202020202020202;
	m_files[2]=0x0404040404040404;
	m_files[3]=0x0808080808080808;
	m_files[4]=0x1010101010101010;
	m_files[5]=0x2020202020202020;
	m_files[6]=0x4040404040404040;
	m_files[7]=BB_RIGHTFILE;

	// Init the variable length rank masks
	m_vrankmasks[0] = 0x1;
	m_vrankmasks[1] = 0x3;
	m_vrankmasks[2] = 0x7;
	m_vrankmasks[3] = 0xf;
	m_vrankmasks[4] = 0x1f;
	m_vrankmasks[5] = 0x3f;
	m_vrankmasks[6] = 0x7f;
	m_vrankmasks[7] = 0xff;
}

//Clear everything for a new game
void Board::newGame() {
	
	init();
	
	//Setup Original Position (Rawls style ;))
	m_allpieces[BB_WHITE]= BB_ROWMASK | BB_ROWMASK << 8;
	m_allpieces[BB_BLACK]= BB_ROWMASK << 48 | BB_ROWMASK << 56;
	m_pbbs[BB_ROOK][BB_WHITE]=get_mask(0)|get_mask(7);
	m_pbbs[BB_ROOK][BB_BLACK]=(get_mask(0)<<56) | (get_mask(7)<<56);
	m_pbbs[BB_KNIGHT][BB_WHITE]=get_mask(1)|get_mask(6);
	m_pbbs[BB_KNIGHT][BB_BLACK]=(get_mask(1)<<56) | (get_mask(6)<<56);
	m_pbbs[BB_BISHOP][BB_WHITE]=get_mask(2)|get_mask(5);
	m_pbbs[BB_BISHOP][BB_BLACK]=(get_mask(2)<<56)|(get_mask(5)<<56);
	m_pbbs[BB_QUEEN][BB_WHITE]=get_mask(3);
	m_pbbs[BB_QUEEN][BB_BLACK]=get_mask(3)<<56;
	m_pbbs[BB_KING][BB_WHITE]=get_mask(4);
	m_pbbs[BB_KING][BB_BLACK]=get_mask(4)<<56;
	m_pbbs[BB_PAWNS][BB_WHITE]=BB_ROWMASK<<8;
	m_pbbs[BB_PAWNS][BB_BLACK]=BB_ROWMASK<<48;
	m_kingpos[BB_WHITE] = 4; m_kingpos[BB_BLACK]=60;

	finalizeSetup(); // Finish off the setup
}

//Clean up setup
void Board::finalizeSetup() {
	int i,turn;

	m_material[0] = m_material[1] = 0;
	m_pt[0] = m_pt[1] = 0;
	for (i = 0; i < 64; i++) {
		if (IS_SET(m_allpieces[BB_WHITE],i))
			turn = BB_WHITE;
		else if (IS_SET(m_allpieces[BB_BLACK],i))
			turn = BB_BLACK;
		else
			continue;

		if (IS_SET(m_pbbs[BB_PAWNS][turn],i)) 
			m_material[turn] += EVAL_PCVAL(BB_PAWNS);
		else if (IS_SET(m_pbbs[BB_KNIGHT][turn],i)) {
			m_material[turn] += EVAL_PCVAL(BB_KNIGHT);
			m_pt[turn] += Evaluator::m_knightpos[i];
		}
		else if (IS_SET(m_pbbs[BB_BISHOP][turn],i)) {
			m_material[turn] += EVAL_PCVAL(BB_BISHOP);
			m_pt[turn] += Evaluator::m_bishoppos[i];
		}
		else if (IS_SET(m_pbbs[BB_ROOK][turn],i)) {
			m_material[turn] += EVAL_PCVAL(BB_ROOK);
		}
		else if (IS_SET(m_pbbs[BB_QUEEN][turn],i)) {
			m_material[turn] += EVAL_PCVAL(BB_QUEEN);
		}
	}

	zobrist(m_zkey,m_pzkey); // Get original Zobrist key
	m_allboard[BB_ROTATE_0] = (m_allpieces[BB_WHITE] | m_allpieces[BB_BLACK]);
	m_allboard[BB_ROTATE_R90] = BB_ZERO;
	m_allboard[BB_ROTATE_R45R] = BB_ZERO;
	m_allboard[BB_ROTATE_R45L] = BB_ZERO;

	m_pbbs[BB_PAWNS_R90][BB_WHITE] = m_pbbs[BB_PAWNS_R90][BB_BLACK] = BB_ZERO;

	// Set up rotated board state
	for (i = 0; i < 64; i++) {
		if (IS_SET(m_allboard[BB_ROTATE_0],i)) {
			m_allboard[BB_ROTATE_R90] |= get_mask(m_R90map[i]);
			m_allboard[BB_ROTATE_R45R] |= get_mask(m_R45Rmap[i]);
			m_allboard[BB_ROTATE_R45L] |= get_mask(m_R45Lmap[i]);
			if (IS_SET(m_pbbs[BB_PAWNS][BB_WHITE],i))
				m_pbbs[BB_PAWNS_R90][BB_WHITE] |= get_mask(m_R90map[i]);
			else if (IS_SET(m_pbbs[BB_PAWNS][BB_BLACK],i))
				m_pbbs[BB_PAWNS_R90][BB_BLACK] |= get_mask(m_R90map[i]);
		}
	}

	//Set up hybrid boards
	m_pbbs[BB_ROOKQUEEN][BB_WHITE] = m_pbbs[BB_ROOK][BB_WHITE] | m_pbbs[BB_QUEEN][BB_WHITE];
	m_pbbs[BB_ROOKQUEEN][BB_BLACK] = m_pbbs[BB_ROOK][BB_BLACK] | m_pbbs[BB_QUEEN][BB_BLACK];
	m_pbbs[BB_BISHOPQUEEN][BB_WHITE] = m_pbbs[BB_BISHOP][BB_WHITE] | m_pbbs[BB_QUEEN][BB_WHITE];
	m_pbbs[BB_BISHOPQUEEN][BB_BLACK] = m_pbbs[BB_BISHOP][BB_BLACK] | m_pbbs[BB_QUEEN][BB_BLACK];
}

int Board::get_kingpos(int color) {
	return m_kingpos[color];
}

bitboard Board::get_signature() {
	return m_zkey;
}

// Applies move and changes state of various (too many) things
// Returns true if the move was a capture
bool Board::move(move_t move) {
	
	bitboard* bb;
	int from = MOVE_GETFROM(move), to = MOVE_GETTO(move), rank;
	bool cap = false;

	m_zkey ^= m_zobristenpawn[m_enpawn+1]; // Get old en pawn zkey out
	m_enpawn = -1; //Reset en passant pawn
	if (m_turn == BB_BLACK) m_nummoves++;

	if (MOVE_ISCASTLE(move)) {
		if (m_turn == BB_WHITE) rank = 0; else rank = 7;
		m_pbbs[BB_KING][m_turn] = CLEAR_BIT(m_pbbs[BB_KING][m_turn],ABSOLUTEC(rank,4));
		m_pbbs[BB_ROOK][m_turn] = CLEAR_BIT(m_pbbs[BB_ROOK][m_turn],ABSOLUTEC(rank,7));
		m_pbbs[BB_ROOKQUEEN][m_turn] = CLEAR_BIT(m_pbbs[BB_ROOKQUEEN][m_turn],ABSOLUTEC(rank,7));
		m_pbbs[BB_KING][m_turn] |= m_masks[ABSOLUTEC(rank,6)];
		m_pbbs[BB_ROOK][m_turn] |= m_masks[ABSOLUTEC(rank,5)];
		m_pbbs[BB_ROOKQUEEN][m_turn] |= m_masks[ABSOLUTEC(rank,5)];
		m_kingpos[m_turn] = ABSOLUTEC(rank,6);
		
		// Update rotated boards
		m_allpieces[m_turn] = CLEAR_BIT(m_allpieces[m_turn],ABSOLUTEC(rank,4));
		m_allpieces[m_turn] = CLEAR_BIT(m_allpieces[m_turn],ABSOLUTEC(rank,7));
		m_allpieces[m_turn] |= m_masks[ABSOLUTEC(rank,5)];
		m_allpieces[m_turn] |= m_masks[ABSOLUTEC(rank,6)];
		m_allboard[BB_ROTATE_R90] = CLEAR_BIT(m_allboard[BB_ROTATE_R90],m_R90map[ABSOLUTEC(rank,4)]);
		m_allboard[BB_ROTATE_R90] = CLEAR_BIT(m_allboard[BB_ROTATE_R90],m_R90map[ABSOLUTEC(rank,7)]);
		m_allboard[BB_ROTATE_R90] |= m_masks[m_R90map[ABSOLUTEC(rank,5)]];
		m_allboard[BB_ROTATE_R90] |= m_masks[m_R90map[ABSOLUTEC(rank,6)]];
		m_allboard[BB_ROTATE_R45R] = CLEAR_BIT(m_allboard[BB_ROTATE_R45R],m_R45Rmap[ABSOLUTEC(rank,4)]);
		m_allboard[BB_ROTATE_R45R] = CLEAR_BIT(m_allboard[BB_ROTATE_R45R],m_R45Rmap[ABSOLUTEC(rank,7)]);
		m_allboard[BB_ROTATE_R45R] |= m_masks[m_R45Rmap[ABSOLUTEC(rank,5)]];
		m_allboard[BB_ROTATE_R45R] |= m_masks[m_R45Rmap[ABSOLUTEC(rank,6)]];
		m_allboard[BB_ROTATE_R45L] = CLEAR_BIT(m_allboard[BB_ROTATE_R45L],m_R45Lmap[ABSOLUTEC(rank,4)]);
		m_allboard[BB_ROTATE_R45L] = CLEAR_BIT(m_allboard[BB_ROTATE_R45L],m_R45Lmap[ABSOLUTEC(rank,7)]);
		m_allboard[BB_ROTATE_R45L] |= m_masks[m_R45Lmap[ABSOLUTEC(rank,5)]];
		m_allboard[BB_ROTATE_R45L] |= m_masks[m_R45Lmap[ABSOLUTEC(rank,6)]];

		// Do Zobrist and castle info
		m_zkey ^= m_zobrist[5][m_turn][ABSOLUTEC(rank,4)];
		m_zkey ^= m_zobrist[5][m_turn][ABSOLUTEC(rank,6)];
		m_zkey ^= m_zobrist[3][m_turn][ABSOLUTEC(rank,7)];
		m_zkey ^= m_zobrist[3][m_turn][ABSOLUTEC(rank,5)];
		m_zkey ^= m_zobristcastle[m_turn][CASTLE_SHORT]; 
		if (m_castle[m_turn][CASTLE_LONG])
			m_zkey ^= m_zobristcastle[m_turn][CASTLE_LONG];

		m_hascastled[m_turn] = true; m_castle[m_turn][CASTLE_SHORT]=false; m_castle[m_turn][CASTLE_LONG]=false;
		m_allboard[BB_ROTATE_0] = (m_allpieces[BB_WHITE] | m_allpieces[BB_BLACK]); // Update the rotators
	}
	else if (MOVE_ISLONGCASTLE(move)) {
		if (m_turn == BB_WHITE) rank = 0; else rank = 7;
		m_pbbs[BB_KING][m_turn] = CLEAR_BIT(m_pbbs[BB_KING][m_turn],ABSOLUTEC(rank,4));
		m_pbbs[BB_ROOK][m_turn] = CLEAR_BIT(m_pbbs[BB_ROOK][m_turn],ABSOLUTEC(rank,0));
		m_pbbs[BB_ROOKQUEEN][m_turn] = CLEAR_BIT(m_pbbs[BB_ROOKQUEEN][m_turn],ABSOLUTEC(rank,0));
		m_pbbs[BB_KING][m_turn] |= m_masks[ABSOLUTEC(rank,2)];
		m_pbbs[BB_ROOK][m_turn] |= m_masks[ABSOLUTEC(rank,3)];
		m_pbbs[BB_ROOKQUEEN][m_turn] |= m_masks[ABSOLUTEC(rank,3)];
		m_kingpos[m_turn] = ABSOLUTEC(rank,2);
		
		//Update rotated boards
		m_allpieces[m_turn] = CLEAR_BIT(m_allpieces[m_turn],ABSOLUTEC(rank,4));
		m_allpieces[m_turn] = CLEAR_BIT(m_allpieces[m_turn],ABSOLUTEC(rank,0));
		m_allpieces[m_turn] |= m_masks[ABSOLUTEC(rank,3)];
		m_allpieces[m_turn] |= m_masks[ABSOLUTEC(rank,2)];
		m_allboard[BB_ROTATE_R90] = CLEAR_BIT(m_allboard[BB_ROTATE_R90],m_R90map[ABSOLUTEC(rank,4)]);
		m_allboard[BB_ROTATE_R90] = CLEAR_BIT(m_allboard[BB_ROTATE_R90],m_R90map[ABSOLUTEC(rank,0)]);
		m_allboard[BB_ROTATE_R90] |= m_masks[m_R90map[ABSOLUTEC(rank,2)]];
		m_allboard[BB_ROTATE_R90] |= m_masks[m_R90map[ABSOLUTEC(rank,3)]];
		m_allboard[BB_ROTATE_R45R] = CLEAR_BIT(m_allboard[BB_ROTATE_R45R],m_R45Rmap[ABSOLUTEC(rank,4)]);
		m_allboard[BB_ROTATE_R45R] = CLEAR_BIT(m_allboard[BB_ROTATE_R45R],m_R45Rmap[ABSOLUTEC(rank,0)]);
		m_allboard[BB_ROTATE_R45R] |= m_masks[m_R45Rmap[ABSOLUTEC(rank,2)]];
		m_allboard[BB_ROTATE_R45R] |= m_masks[m_R45Rmap[ABSOLUTEC(rank,3)]];
		m_allboard[BB_ROTATE_R45L] = CLEAR_BIT(m_allboard[BB_ROTATE_R45L],m_R45Lmap[ABSOLUTEC(rank,4)]);
		m_allboard[BB_ROTATE_R45L] = CLEAR_BIT(m_allboard[BB_ROTATE_R45L],m_R45Lmap[ABSOLUTEC(rank,0)]);
		m_allboard[BB_ROTATE_R45L] |= m_masks[m_R45Lmap[ABSOLUTEC(rank,2)]];
		m_allboard[BB_ROTATE_R45L] |= m_masks[m_R45Lmap[ABSOLUTEC(rank,3)]];
		
		//Update Zobrist info
		m_zkey ^= m_zobrist[5][m_turn][ABSOLUTEC(rank,4)];
		m_zkey ^= m_zobrist[5][m_turn][ABSOLUTEC(rank,2)];
		m_zkey ^= m_zobrist[3][m_turn][ABSOLUTEC(rank,0)];
		m_zkey ^= m_zobrist[3][m_turn][ABSOLUTEC(rank,3)];
		if (m_castle[m_turn][CASTLE_SHORT])
			m_zkey ^= m_zobristcastle[m_turn][CASTLE_SHORT]; 

		m_zkey ^= m_zobristcastle[m_turn][CASTLE_LONG];

		m_hascastled[m_turn] = true; m_castle[m_turn][CASTLE_LONG]=false; m_castle[m_turn][CASTLE_SHORT]=false;
		m_allboard[BB_ROTATE_0] = (m_allpieces[BB_WHITE] | m_allpieces[BB_BLACK]); // Update the rotators
	}
	else {
		// Get the bitboard of the moved piece
		if (IS_SET(m_pbbs[BB_PAWNS][m_turn],from)) {
			bb = &m_pbbs[BB_PAWNS][m_turn];
			
			m_zkey ^= m_zobrist[0][m_turn][from]; // Remove zobrist from key
			m_zkey ^= m_zobrist[0][m_turn][to];
			m_pzkey ^= m_zobrist[0][m_turn][from];
			m_pzkey ^= m_zobrist[0][m_turn][to];

			//Update rotated pawn bitboard
			m_pbbs[BB_PAWNS_R90][m_turn] = CLEAR_BIT(m_pbbs[BB_PAWNS_R90][m_turn],m_R90map[from]);
			m_pbbs[BB_PAWNS_R90][m_turn] |= m_masks[m_R90map[to]];

			if (ABS(RANK(from)-RANK(to)) == 2) // Set the en passant pawn
				m_enpawn = to;
			// En passant
			else if ((FILE(from) != FILE(to)) && !is_occupied(to)) {
				if (m_turn == BB_WHITE) {
					cap = true;
					clear_position(BB_BLACK,to-8);
				}
				else {
					cap = true;
					clear_position(BB_WHITE,to+8);
				}
			}
		}
		else if (IS_SET(m_pbbs[BB_KNIGHT][m_turn],from)) {
			bb = &m_pbbs[BB_KNIGHT][m_turn];
			m_zkey ^= m_zobrist[1][m_turn][from]; // Remove zobrist from key
			m_zkey ^= m_zobrist[1][m_turn][to];
			m_pt[m_turn] -= Evaluator::m_knightpos[from]; //Update piece table info
			m_pt[m_turn] += Evaluator::m_knightpos[to];
		}
		else if (IS_SET(m_pbbs[BB_BISHOP][m_turn],from)) {
			bb = &m_pbbs[BB_BISHOP][m_turn];

			//Update bishop-queen hybrid
			m_pbbs[BB_BISHOPQUEEN][m_turn] = CLEAR_BIT(m_pbbs[BB_BISHOPQUEEN][m_turn],from);
			m_pbbs[BB_BISHOPQUEEN][m_turn] |= m_masks[to];

			m_zkey ^= m_zobrist[2][m_turn][from]; // Remove zobrist from key
			m_zkey ^= m_zobrist[2][m_turn][to];
			m_pt[m_turn] -= Evaluator::m_bishoppos[from]; //Update piece table info
			m_pt[m_turn] += Evaluator::m_bishoppos[to];
		}
		else if (IS_SET(m_pbbs[BB_ROOK][m_turn],from)) {
			bb = &m_pbbs[BB_ROOK][m_turn];

			//Update rook-queen hybrid
			m_pbbs[BB_ROOKQUEEN][m_turn] = CLEAR_BIT(m_pbbs[BB_ROOKQUEEN][m_turn],from);
			m_pbbs[BB_ROOKQUEEN][m_turn] |= m_masks[to];

			m_zkey ^= m_zobrist[3][m_turn][from]; // Remove zobrist from key
			m_zkey ^= m_zobrist[3][m_turn][to];
			if (m_turn == BB_WHITE) {
				if (from == 0) {
					if (m_castle[m_turn][CASTLE_LONG])
						m_zkey ^= m_zobristcastle[m_turn][CASTLE_LONG];
					m_castle[m_turn][CASTLE_LONG] = false; //Queen rook move negates long castle	
				}
				else if (from == 7) {
					if (m_castle[m_turn][CASTLE_SHORT])
						m_zkey ^= m_zobristcastle[m_turn][CASTLE_SHORT];
					m_castle[m_turn][CASTLE_SHORT] = false; //King rook move negates short castle
				}
			}
			else {
				if (from == 56) {
					if (m_castle[m_turn][CASTLE_LONG])
						m_zkey ^= m_zobristcastle[m_turn][CASTLE_LONG];
					m_castle[m_turn][CASTLE_LONG] = false; //Queen rook move negates long castle
				}
				else if (from == 63) {
					if (m_castle[m_turn][CASTLE_SHORT])
						m_zkey ^= m_zobristcastle[m_turn][CASTLE_SHORT];
					m_castle[m_turn][CASTLE_SHORT] = false; //King rook move negates short castle
				}
			}
			
		}
		else if (IS_SET(m_pbbs[BB_QUEEN][m_turn],from)) {
			bb = &m_pbbs[BB_QUEEN][m_turn];

			//Update hybrids
			m_pbbs[BB_BISHOPQUEEN][m_turn] = CLEAR_BIT(m_pbbs[BB_BISHOPQUEEN][m_turn],from);
			m_pbbs[BB_BISHOPQUEEN][m_turn] |= m_masks[to];
			m_pbbs[BB_ROOKQUEEN][m_turn] = CLEAR_BIT(m_pbbs[BB_ROOKQUEEN][m_turn],from);
			m_pbbs[BB_ROOKQUEEN][m_turn] |= m_masks[to];

			m_zkey ^= m_zobrist[4][m_turn][from]; // Remove zobrist from key
			m_zkey ^= m_zobrist[4][m_turn][to];
		}
		else if (IS_SET(m_pbbs[BB_KING][m_turn],from)) {
			bb = &m_pbbs[BB_KING][m_turn];
			m_kingpos[m_turn] = to;
			m_zkey ^= m_zobrist[5][m_turn][from]; // Remove zobrist from key
			m_zkey ^= m_zobrist[5][m_turn][to];
			if (m_castle[m_turn][CASTLE_LONG])
				m_zkey ^= m_zobristcastle[m_turn][CASTLE_LONG];
			if (m_castle[m_turn][CASTLE_SHORT])
				m_zkey ^= m_zobristcastle[m_turn][CASTLE_SHORT];
			m_castle[m_turn][CASTLE_SHORT] = m_castle[m_turn][CASTLE_LONG] = false; // If the king moves castling is over
		}
		
		// Apply the move
		*bb = CLEAR_BIT(*bb,from);
		*bb |= m_masks[to];
		if (is_occupied(to)) {
			cap = true;
			clear_position(otherTurn(),to);
		}
		
		// Update rotators with move info
		m_allpieces[m_turn] = CLEAR_BIT(m_allpieces[m_turn],from);
		m_allpieces[m_turn] |= m_masks[to];
		m_allboard[BB_ROTATE_R90] = CLEAR_BIT(m_allboard[BB_ROTATE_R90],m_R90map[from]);
		m_allboard[BB_ROTATE_R90] |= m_masks[m_R90map[to]];
		m_allboard[BB_ROTATE_R45R] = CLEAR_BIT(m_allboard[BB_ROTATE_R45R],m_R45Rmap[from]);
		m_allboard[BB_ROTATE_R45R] |= m_masks[m_R45Rmap[to]];
		m_allboard[BB_ROTATE_R45L] = CLEAR_BIT(m_allboard[BB_ROTATE_R45L],m_R45Lmap[from]);
		m_allboard[BB_ROTATE_R45L] |= m_masks[m_R45Lmap[to]];
		
		// Promotions
		if (MOVE_ISPROMOTION(move)) {
			m_pbbs[BB_PAWNS][m_turn] = CLEAR_BIT(m_pbbs[BB_PAWNS][m_turn],to);
			m_pbbs[BB_PAWNS_R90][m_turn] = CLEAR_BIT(m_pbbs[BB_PAWNS_R90][m_turn],m_R90map[to]);
			m_zkey ^= m_zobrist[0][m_turn][to];
			m_pzkey ^= m_zobrist[0][m_turn][to];
			switch (MOVE_GETPROMOTION(move)) {
			case BB_KNIGHT:
				m_pbbs[BB_KNIGHT][m_turn] |= m_masks[to];
				m_zkey ^= m_zobrist[1][m_turn][to];
				m_material[m_turn] += EVAL_PCVAL(BB_KNIGHT)-EVAL_PCVAL(BB_PAWNS);
				m_pt[m_turn] += Evaluator::m_knightpos[to]; //Update piece table info
				break;
			case BB_BISHOP:
				m_pbbs[BB_BISHOP][m_turn] |= m_masks[to];
				m_pbbs[BB_BISHOPQUEEN][m_turn] |= m_masks[to];
				m_zkey ^= m_zobrist[2][m_turn][to];
				m_material[m_turn] += EVAL_PCVAL(BB_BISHOP)-EVAL_PCVAL(BB_PAWNS);
				m_pt[m_turn] += Evaluator::m_bishoppos[to]; //Update piece table info
				break;
			case BB_ROOK:
				m_pbbs[BB_ROOK][m_turn] |= m_masks[to];
				m_pbbs[BB_ROOKQUEEN][m_turn] |= m_masks[to];
				m_zkey ^= m_zobrist[3][m_turn][to];
				m_material[m_turn] += EVAL_PCVAL(BB_ROOK)-EVAL_PCVAL(BB_PAWNS);
				break;
			case BB_QUEEN:
				m_pbbs[BB_QUEEN][m_turn] |= m_masks[to];
				m_pbbs[BB_ROOKQUEEN][m_turn] |= m_masks[to];
				m_pbbs[BB_BISHOPQUEEN][m_turn] |= m_masks[to];
				m_zkey ^= m_zobrist[4][m_turn][to];
				m_material[m_turn] += EVAL_PCVAL(BB_QUEEN)-EVAL_PCVAL(BB_PAWNS);
				break;
			}
		}
		m_allboard[BB_ROTATE_0] = (m_allpieces[BB_WHITE] | m_allpieces[BB_BLACK]);
	}
	
	// The Zobrist turn key
	m_zkey ^= m_zobristturn[m_turn]; m_zkey ^= m_zobristturn[otherTurn()];

	// The Zobrist en passant key
	m_zkey ^= m_zobristenpawn[m_enpawn+1];

	m_turn = otherTurn(); //Switch turn

	return cap;
}

void Board::get_square(int pos, BoardPosition& bp) const {

	if (IS_SET(m_allpieces[BB_WHITE],pos))
		bp.m_color = BB_WHITE;
	else if (IS_SET(m_allpieces[BB_BLACK],pos))
		bp.m_color = BB_BLACK;
	else {
		bp.m_color = BB_DRAW;
		return;
	}
	if (IS_SET(m_pbbs[BB_PAWNS][bp.m_color],pos))
		bp.m_type = BB_PAWNS;
	else if (IS_SET(m_pbbs[BB_KNIGHT][bp.m_color],pos))
		bp.m_type = BB_KNIGHT;
	else if (IS_SET(m_pbbs[BB_BISHOP][bp.m_color],pos))
		bp.m_type = BB_BISHOP;
	else if (IS_SET(m_pbbs[BB_ROOK][bp.m_color],pos))
		bp.m_type = BB_ROOK;
	else if (IS_SET(m_pbbs[BB_QUEEN][bp.m_color],pos))
		bp.m_type = BB_QUEEN;
	else if (IS_SET(m_pbbs[BB_KING][bp.m_color],pos))
		bp.m_type = BB_KING;
}		

bitboard Board::get_bitboard(int color, int type) {

	switch (type) {
	case BB_PAWNS:
		return m_pbbs[BB_PAWNS][color];
	case BB_KNIGHT:
		return m_pbbs[BB_KNIGHT][color];
	case BB_BISHOP:
		return m_pbbs[BB_BISHOP][color];
	case BB_ROOK:
		return m_pbbs[BB_ROOK][color];
	case BB_QUEEN:
		return m_pbbs[BB_QUEEN][color];
	case BB_KING:
		return m_pbbs[BB_KING][color];
	};

	return 0;
}

int Board::num_castles(int color) {
	int castles=0;
	if (m_castle[color][CASTLE_SHORT])
		castles++;
	if (m_castle[color][CASTLE_LONG])
		castles++;
	
	return castles;
}

bool Board::is_castled(int color) {
	return m_hascastled[color];	
}

//Eliminate a position
void Board::clear_position(int color, int pos) {

//	if (IS_SET(m_allpieces[color],pos)) {
		
		//Take care of rotator bitboards
		m_allpieces[color] = CLEAR_BIT(m_allpieces[color],pos);
		m_allboard[BB_ROTATE_R90] = CLEAR_BIT(m_allboard[BB_ROTATE_R90],m_R90map[pos]);
		m_allboard[BB_ROTATE_R45R] = CLEAR_BIT(m_allboard[BB_ROTATE_R45R],m_R45Rmap[pos]);
		m_allboard[BB_ROTATE_R45L] = CLEAR_BIT(m_allboard[BB_ROTATE_R45L],m_R45Lmap[pos]);
		
		//Take care of piece bitboards
		if (IS_SET(m_pbbs[BB_PAWNS][color],pos)) {
			m_zkey ^= m_zobrist[0][color][pos];
			m_pzkey ^= m_zobrist[0][color][pos];
			m_pbbs[BB_PAWNS][color] = CLEAR_BIT(m_pbbs[BB_PAWNS][color],pos);
			m_pbbs[BB_PAWNS_R90][color] = CLEAR_BIT(m_pbbs[BB_PAWNS_R90][color],m_R90map[pos]);
			m_material[color] -= EVAL_PCVAL(BB_PAWNS);
		}
		else if (IS_SET(m_pbbs[BB_KNIGHT][color],pos)) {
			m_zkey ^= m_zobrist[1][color][pos];
			m_pbbs[BB_KNIGHT][color] = CLEAR_BIT(m_pbbs[BB_KNIGHT][color],pos);
			m_material[color] -= EVAL_PCVAL(BB_KNIGHT);
			m_pt[color] -= Evaluator::m_knightpos[pos]; //Update piece table info
		}
		else if (IS_SET(m_pbbs[BB_BISHOP][color],pos)) {
			m_zkey ^= m_zobrist[2][color][pos];
			m_pbbs[BB_BISHOP][color] = CLEAR_BIT(m_pbbs[BB_BISHOP][color],pos);
			m_pbbs[BB_BISHOPQUEEN][color] = CLEAR_BIT(m_pbbs[BB_BISHOPQUEEN][color],pos);
			m_material[color] -= EVAL_PCVAL(BB_BISHOP);
			m_pt[color] -= Evaluator::m_bishoppos[pos]; //Update piece table info
		} 
		else if (IS_SET(m_pbbs[BB_ROOK][color],pos)) {
			m_zkey ^= m_zobrist[3][color][pos];
			m_pbbs[BB_ROOK][color] = CLEAR_BIT(m_pbbs[BB_ROOK][color],pos);
			m_pbbs[BB_ROOKQUEEN][color] = CLEAR_BIT(m_pbbs[BB_ROOKQUEEN][color],pos);
			m_material[color] -= EVAL_PCVAL(BB_ROOK);
		}
		else if (IS_SET(m_pbbs[BB_QUEEN][color],pos)) {
			m_zkey ^= m_zobrist[4][color][pos];
			m_pbbs[BB_QUEEN][color] = CLEAR_BIT(m_pbbs[BB_QUEEN][color],pos);
			m_pbbs[BB_ROOKQUEEN][color] = CLEAR_BIT(m_pbbs[BB_ROOKQUEEN][color],pos);
			m_pbbs[BB_BISHOPQUEEN][color] = CLEAR_BIT(m_pbbs[BB_BISHOPQUEEN][color],pos);
			m_material[color] -= EVAL_PCVAL(BB_QUEEN);
		}
		else if (IS_SET(m_pbbs[BB_KING][color],pos)) {
			m_zkey ^= m_zobrist[5][color][pos];
			m_pbbs[BB_KING][color] = CLEAR_BIT(m_pbbs[BB_KING][color],pos);
		}
		//else
	//		cout << "Severe Warning: Clearing Nothing" << endl;
//	}
}

bool Board::is_occupied(int pos) const {
	return IS_SET(m_allboard[BB_ROTATE_0],pos);
}

//Determines if current player is checking opponent
bool Board::in_check() {

	bitboard atx;
	int oturn;

	toggleTurn();
	oturn = otherTurn();
	atx = get_knight_attacks(m_kingpos[m_turn]);
	if ((atx & m_pbbs[BB_KNIGHT][oturn])) { toggleTurn(); return true; }

	atx = get_bishop_attacks(m_kingpos[m_turn]);
	if ((atx & (m_pbbs[BB_BISHOP][oturn] | m_pbbs[BB_QUEEN][oturn]))) { toggleTurn(); return true; }

	atx = get_rook_attacks(m_kingpos[m_turn]);
	if ((atx & (m_pbbs[BB_ROOK][oturn] | m_pbbs[BB_QUEEN][oturn]))) { toggleTurn(); return true; }

	atx = get_single_pawn_attacks(m_kingpos[m_turn]);
	if ((atx & m_pbbs[BB_PAWNS][oturn])) { toggleTurn(); return true; }

	atx = get_king_attacks(m_kingpos[m_turn]);
	if ((atx & m_pbbs[BB_KING][oturn])) { toggleTurn(); return true; }

	toggleTurn();
	return false;
}

int Board::turn() const {
	return m_turn;
}

void Board::toggleTurn() {
	m_zkey ^= m_zobristturn[m_turn]; m_zkey ^= m_zobristturn[otherTurn()];
	m_turn = otherTurn();
}

//Determine if game has winner
int Board::winner() {
	
	MoveList moves,fmoves;

	if (check()) {
		generate_moves(fmoves);
		generate_check_filter(moves,fmoves);
		if (moves.size() == 0) return otherTurn();
	}
	moves.clear(); fmoves.clear();
	toggleTurn();
	if (check()) {
		generate_moves(fmoves);
		generate_check_filter(moves,fmoves);
		if (moves.size() == 0) { toggleTurn(); return m_turn; }
	}
	toggleTurn();

	return BB_DRAW;
}

//Check for stalemate
bool Board::stalemate() {
	MoveList moves,fmoves;

	if (!check()) {
		generate_moves(fmoves);
		generate_check_filter(moves,fmoves);
		if (moves.size() == 0) return true;
	}
	moves.clear(); fmoves.clear();
	toggleTurn();
	if (!check()) {
		generate_moves(fmoves);
		generate_check_filter(moves,fmoves);
		if (moves.size() == 0) { toggleTurn(); return true; }
	}
	toggleTurn();

	return false;
}

// Tests to see if current player is in check
bool Board::check() {
	bool check;
	toggleTurn();
	check = in_check();
	toggleTurn();
	return check;
}

int Board::get_nummoves() const {
	return m_nummoves;
}

int Board::get_gamestage() const {
	if (m_nummoves <= 18)
		return GAME_OPENING;
	if (m_material[0] + m_material[1] <= 3800)
		return GAME_ENDING;
	
	return GAME_MIDDLE;
}

int Board::get_piecetable(int side) const {
	return m_pt[side];
}

int Board::get_material(int side) const {
	return m_material[side];
}

int Board::mat_gain(move_t move) const {
	int to;
	
	to = MOVE_GETTO(move);
	if (is_occupied(to)) {
		BoardPosition bp;
		get_square(to,bp);

		return EVAL_PCVAL(bp.m_type);
	}
	
	return 0;
}

int Board::otherTurn() const {
	return 1-m_turn;
}

//Testing function
void Board::verify_rotator(move_t move) {

	bitboard vzkey,vpzkey,vR90,vR45R,vR45L,vpR90[2];
	int i;

	zobrist(vzkey,vpzkey); // Get original Zobrist key
	vR90 = BB_ZERO;
	vR45R = BB_ZERO;
	vR45L = BB_ZERO;
	vpR90[0] = vpR90[1] = BB_ZERO;
	// Set up rotated board state
	for (i = 0; i < 64; i++) {
		if (IS_SET(m_allboard[BB_ROTATE_0],i)) {
			vR90 |= get_mask(m_R90map[i]);
			vR45R |= get_mask(m_R45Rmap[i]);
			vR45L |= get_mask(m_R45Lmap[i]);
			if (IS_SET(m_pbbs[BB_PAWNS][0],i))
				vpR90[0] |= get_mask(m_R90map[i]);
			else if (IS_SET(m_pbbs[BB_PAWNS][1],i))
				vpR90[1] |= get_mask(m_R90map[i]);
		}
	}
	if (vR90 != m_allboard[BB_ROTATE_R90]) {
		cout << "Error: 90 rotate out of sync!" << endl;
		cout << this->toString() << endl;
		showbitboard(vR90);
		showbitboard(m_allboard[BB_ROTATE_R90]);
	//	exit(0);
	}
	if (vR45R != m_allboard[BB_ROTATE_R45R])
		cout << "Error: 45R rotate out of sync!" << endl;
	if (vR45L != m_allboard[BB_ROTATE_R45L])
		cout << "Error: 45L rotate out of sync!" << endl;

	if (m_zkey != vzkey) {
		cout << "Error; Zobrist out of sync!" << endl;
		cout << Notation::MoveToSAN(move,*this) << endl;
		cout << toString() << endl;
	}
	if (vpR90[0] != m_pbbs[BB_PAWNS_R90][0]) {
		cout << "Error: Rotated Pawn Bitboard out of sync!" << endl;
		showbitboard(m_pbbs[BB_PAWNS_R90][0]);
	}
	if (vpR90[1] != m_pbbs[BB_PAWNS_R90][1])
		cout << "Error: Rotated Pawn Bitboard out of sync!" << endl;

	if (m_pbbs[BB_ROOKQUEEN][BB_WHITE] != (m_pbbs[BB_ROOK][BB_WHITE] | m_pbbs[BB_QUEEN][BB_WHITE]) ||
	    m_pbbs[BB_ROOKQUEEN][BB_BLACK] != (m_pbbs[BB_ROOK][BB_BLACK] | m_pbbs[BB_QUEEN][BB_BLACK]))
		cout << "Error: RookQueen hybrid bitboard out of sync!" << endl;

	if (m_pbbs[BB_BISHOPQUEEN][BB_WHITE] != (m_pbbs[BB_BISHOP][BB_WHITE] | m_pbbs[BB_QUEEN][BB_WHITE]) ||
	    m_pbbs[BB_BISHOPQUEEN][BB_BLACK] != (m_pbbs[BB_BISHOP][BB_BLACK] | m_pbbs[BB_QUEEN][BB_BLACK]))
		cout << "Error: BishopQueen hybrid bitboard out of sync!" << endl;

	if (vpzkey != m_pzkey) {
		cout << "Error: Pawn Zobrist out of sync!" << endl;
	//	cout << Notation::MoveToSAN(move,*this) << endl;
		cout << toString() << endl;
	}
}

//Get directiosn information for each square
void Board::init_directions() {

	int i,rank,file;
	
	//Init directions
	for (rank = 0; rank < 64; rank++) for (file = 0; file < 64; file++) 
		m_directions[rank][file] = BB_NORELATION;
	//Zero bitboards
	for (rank = 0; rank < 64; rank++) for (file = 0; file < 8; file++)
		m_dirbb[rank][file] = BB_ZERO;

	//Get directions
	for (i = 0; i < 64; i++) {
		//Span in upper left direction
		for (rank = RANK(i), file = FILE(i); rank < 8 && file >= 0; rank++, file--) {
			m_directions[i][ABSOLUTEC(rank,file)] = BB_UPLEFT;
			m_dirbb[i][BB_UPLEFT] |= get_mask(ABSOLUTEC(rank,file));
		}
		//Span in upper right direction
		for (rank = RANK(i), file = FILE(i); rank < 8 && file < 8; rank++, file++) {
			m_directions[i][ABSOLUTEC(rank,file)] = BB_UPRIGHT;
			m_dirbb[i][BB_UPRIGHT] |= get_mask(ABSOLUTEC(rank,file));
		}
		//Span in lower left direction
		for (rank = RANK(i), file = FILE(i); rank >= 0 && file >= 0; rank--, file--) {
			m_directions[i][ABSOLUTEC(rank,file)] = BB_DOWNLEFT;
			m_dirbb[i][BB_DOWNLEFT] |= get_mask(ABSOLUTEC(rank,file));
		}
		//Span in lower right direction
		for (rank = RANK(i), file = FILE(i); rank >= 0 && file < 8; rank--, file++) {
			m_directions[i][ABSOLUTEC(rank,file)] = BB_DOWNRIGHT;
			m_dirbb[i][BB_DOWNRIGHT] |= get_mask(ABSOLUTEC(rank,file));
		}
		//Span in lower direction
		for (rank = RANK(i), file = FILE(i); rank >= 0; rank--) {
			m_directions[i][ABSOLUTEC(rank,file)] = BB_DOWN;
			m_dirbb[i][BB_DOWN] |= get_mask(ABSOLUTEC(rank,file));
		}
		//Span in upper direction
		for (rank = RANK(i), file = FILE(i); rank < 8; rank++) {
			m_directions[i][ABSOLUTEC(rank,file)] = BB_UP;
			m_dirbb[i][BB_UP] |= get_mask(ABSOLUTEC(rank,file));
		}
		//Span in right direction
		for (rank = RANK(i), file = FILE(i); file < 8; file++) {
			m_directions[i][ABSOLUTEC(rank,file)] = BB_RIGHT;
			m_dirbb[i][BB_RIGHT] |= get_mask(ABSOLUTEC(rank,file));
		}
		//Span in left direction
		for (rank = RANK(i), file = FILE(i); file >= 0; file--) {
			m_directions[i][ABSOLUTEC(rank,file)] = BB_LEFT;
			m_dirbb[i][BB_LEFT] |= get_mask(ABSOLUTEC(rank,file));
		}
	}

}

void Board::showbitboard(bitboard bb) const {
	
	int rank,file;
	for (rank = 7; rank >= 0; rank--) {
		for (file = 0; file < 8; file++) {
			if (IS_SET(bb,8*rank+file)) cout << "1 ";
			else cout << "0 ";
		}
		cout << endl;
	}
	cout << endl;
}

string Board::toString() const {
	int rank,file;
	int turn=0;
	char conv[2];
	string str="";
	for (rank = 7; rank >= 0; rank--) {
		for (file = 0; file < 8; file++) {
			if (IS_SET(m_allpieces[BB_WHITE],ABSOLUTEC(rank,file)))
				turn = BB_WHITE;
			else if (IS_SET(m_allpieces[BB_BLACK],ABSOLUTEC(rank,file)))
				turn = BB_BLACK;
			else {
				str += "_";
			}
			
			if (IS_SET(m_pbbs[BB_PAWNS][turn],ABSOLUTEC(rank,file))) {
				if (turn == BB_WHITE) str += "P";
				else str += "p";
			}
			else if (IS_SET(m_pbbs[BB_KNIGHT][turn],ABSOLUTEC(rank,file))) {
				if (turn == BB_WHITE) str += "N";
				else str += "n";
			}
			else if (IS_SET(m_pbbs[BB_BISHOP][turn],ABSOLUTEC(rank,file))) {
				if (turn == BB_WHITE) str += "B";
				else str += "b";
			}
			else if (IS_SET(m_pbbs[BB_ROOK][turn],ABSOLUTEC(rank,file))) {
				if (turn == BB_WHITE) str += "R";
				else str += "r";
			}
			else if (IS_SET(m_pbbs[BB_QUEEN][turn],ABSOLUTEC(rank,file))) {
				if (turn == BB_WHITE) str += "Q";
				else str += "q";
			}
			else if (IS_SET(m_pbbs[BB_KING][turn],ABSOLUTEC(rank,file))) {
				if (turn == BB_WHITE) str += "K";
				else str += "k";
			}
			str += "|";	
		}
		conv[0] = rank+'0'+1; conv[1]='\0';
		str += string(conv);
		str += "\n";
	}
	for (rank = 0; rank < 8; rank++) {
		conv[0] = rank+'a'; conv[1]='\0';
		str += string(conv)+"|";	
	}
	str += "\nFEN: "+toFEN();
	return str+"\n";
}
