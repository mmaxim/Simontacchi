//Mike Maxim
//Bitboard move generator
//A lesson in bit manipulation

#include <board.h>
#include <notation.h>

bitboard Board::m_knightatx[64],Board::m_rankatx[64][256],Board::m_fileatx[64][256];
bitboard Board::m_kingatx[64],Board::m_diagatxA1H8[64][256],Board::m_diagatxA8H1[64][256];
bitboard Board::m_masks[64],Board::m_clearmasks[64],Board::m_files[8],Board::m_vrankmasks[8];

// Generate the pseudo-legal moves (moves resulting in check for current player are generated)
void Board::generate_moves(MoveList& moves) {

	generate_pawn_moves(moves, BB_ALL);
	generate_knight_moves(moves);
	generate_king_moves(moves, BB_ALL);
	generate_bishop_moves(moves,true);
	generate_rook_moves(moves,true);

}

//Generate pseudo-legal non-captures
void Board::generate_non_capture_moves(MoveList& moves) {

	bitboard opieces = ~m_allpieces[1-m_turn];

	generate_pawn_moves(moves,opieces);
	generate_knight_moves(moves,opieces);
	generate_king_moves(moves,opieces);
	generate_bishop_moves(moves,true,opieces);
	generate_rook_moves(moves,true,opieces);
}

//Generate pseudo-legal captures
void Board::generate_capture_moves(MoveList& moves) {

	bitboard opieces = m_allpieces[1-m_turn];

	generate_pawn_moves(moves,opieces,false);
	generate_knight_moves(moves,opieces);
	generate_king_moves(moves,opieces,false);
	generate_bishop_moves(moves,true,opieces);
	generate_rook_moves(moves,true,opieces);
}

//Filter the legal moves by check causation (never used during search)
void Board::generate_check_filter(MoveList& moves, const MoveList& unchkmoves) {
	int i;
	Board sboard;
	move_t mmove;

	sboard = *this;
	for (i = 0; i < unchkmoves.size(); i++) {
		mmove = unchkmoves.get(i);
		move(mmove);
		if (!in_check())
			moves.insert(mmove);
		*this = sboard;
	}
}

// Get all attacks for current player
bitboard Board::get_all_attacks() {
	
	int pos;
	bitboard attacks,bb;
	
	attacks = get_pawn_attacks();

	bb = m_pbbs[BB_KNIGHT][m_turn];
	while (bb) {
		attacks |= get_knight_attacks((pos = BB_NEXTBIT(bb)));
		bb = CLEAR_BIT(bb,pos);
	}
	bb = m_pbbs[BB_BISHOP][m_turn];
	while (bb) {
		attacks |= get_bishop_attacks((pos = BB_NEXTBIT(bb)));
		bb = CLEAR_BIT(bb,pos);
	}
	bb = m_pbbs[BB_ROOK][m_turn];
	while (bb) {
		attacks |= get_rook_attacks((pos = BB_NEXTBIT(bb)));
		bb = CLEAR_BIT(bb,pos);
	}
	bb = m_pbbs[BB_QUEEN][m_turn];
	while (bb) {
		pos = BB_NEXTBIT(bb);
		attacks |= get_rook_attacks(pos) | get_bishop_attacks(pos);
		bb = CLEAR_BIT(bb,pos);
	}
	
	attacks |= get_king_attacks(m_kingpos[m_turn]);

	return attacks;
}

//Get all attacks from a square
bitboard Board::get_all_attacks_to(int pos) {
	bitboard atx;

	atx = ((get_knight_attacks_own(pos) & (m_pbbs[BB_KNIGHT][0] | m_pbbs[BB_KNIGHT][1])) | 
		  (get_bishop_attacks_own(pos) & (m_pbbs[BB_BISHOPQUEEN][0] | m_pbbs[BB_BISHOPQUEEN][1])) |
		  (get_rook_attacks_own(pos) & (m_pbbs[BB_ROOKQUEEN][0] | m_pbbs[BB_ROOKQUEEN][1])) |
		  (get_king_attacks_own(pos) & (m_pbbs[BB_KING][0] | m_pbbs[BB_KING][1])) |
		  (get_single_pawn_attacks_own(pos) & (m_pbbs[BB_PAWNS][0] | m_pbbs[BB_PAWNS][1])));

	return atx;
}

//Check legality of a move
bool Board::validate_move(move_t move) {

	BoardPosition from,to;
	bitboard atx = BB_ZERO;
	int cfrom,cto,piece;

	if (!MOVE_ISBLANKMOVE(move) && !MOVE_ISCASTLE(move) && !MOVE_ISLONGCASTLE(move)) {
		
		cfrom = MOVE_GETFROM(move); cto = MOVE_GETTO(move); piece = MOVE_GETPIECE(move);
		get_square(cfrom,from);
		get_square(cto,to);

		//Immediate fails
		if (piece != from.m_type || m_turn != from.m_color || to.m_color == m_turn)
			return false;

		//Pawn checks
		if (piece == BB_PAWNS) {
			//Phantom capture
			if (to.m_color == BB_DRAW && FILE(cfrom) != FILE(cto)) 
				return false;
			//Piece blocking a 2 space move
			if ((m_turn == BB_WHITE && (RANK(cfrom) < RANK(cto)-1) && is_occupied(cto-8)) ||
				(m_turn == BB_BLACK && (RANK(cfrom) > RANK(cto)+1) && is_occupied(cto+8)))
				return false;
			//Move straight onto a piece
			if (to.m_color != BB_DRAW && FILE(cfrom) == FILE(cto))
				return false;
			//Wrong direction
			if ((m_turn == BB_WHITE && RANK(cfrom) > RANK(cto)) ||
				(m_turn == BB_BLACK && RANK(cfrom) < RANK(cto)))
				return false;

			return true;
		}
		else {
			//Get attacks from the square to make sure sliding pieces dont move through others
			if (piece == BB_BISHOP) {
				atx = get_bishop_attacks(cfrom);
			}
			else if (piece == BB_ROOK) {
				atx = get_rook_attacks(cfrom);
			}
			else if (piece == BB_QUEEN) {
				atx = get_bishop_attacks(cfrom) | get_rook_attacks(cfrom);
			}
			else
				atx = BB_ALL;

			if (!IS_SET(atx, cto))
				return false;
		}

		return true;
	}

	return false;
}

// Get castles
void Board::generate_castle_moves(MoveList& moves, int pos) {

	Board sboard;
	move_t scastle = MOVE_EMPTY,lcastle = MOVE_EMPTY;
	int rank = RANK(pos), file = FILE(pos);
	bitboard attacks;

	MOVE_SETCASTLE(scastle); MOVE_SETLONGCASTLE(lcastle);
	MOVE_SETPIECE(scastle,BB_KING); MOVE_SETPIECE(lcastle,BB_KING);
	//Determine short castle
	if (m_castle[m_turn][CASTLE_SHORT] && !is_occupied(ABSOLUTEC(rank,file+1)) && !is_occupied(ABSOLUTEC(rank,file+2)) &&
	    IS_SET(m_pbbs[BB_ROOK][m_turn],ABSOLUTEC(rank,file+3))) {
		
		m_turn = otherTurn();
		attacks = get_all_attacks();
		if (!IS_SET(attacks,ABSOLUTEC(rank,file+1)) && !IS_SET(attacks,ABSOLUTEC(rank,file+2))) {
			moves.insert(scastle);
		}
		m_turn = otherTurn();
	}
	if (m_castle[m_turn][CASTLE_LONG] && !is_occupied(ABSOLUTEC(rank,file-1)) && !is_occupied(ABSOLUTEC(rank,file-2)) &&
	    !is_occupied(ABSOLUTEC(rank,file-3)) && IS_SET(m_pbbs[BB_ROOK][m_turn],ABSOLUTEC(rank,file-4))) {
	
		m_turn = otherTurn();
		attacks = get_all_attacks();
		if (!IS_SET(attacks,ABSOLUTEC(rank,file-1)) && !IS_SET(attacks,ABSOLUTEC(rank,file-2))) {
			moves.insert(lcastle);
		}
		m_turn = otherTurn();
	}
}

// Get all types of pawn moves
void Board::generate_pawn_moves(MoveList& moves, bitboard opp, bool noncap) {

	move_t move = MOVE_EMPTY;
	int oturn = otherTurn();
	if (m_turn == BB_WHITE) {
		//Get 1 moves	
		if (noncap)
			interp_pawn_bitboard(moves,(m_pbbs[BB_PAWNS][m_turn]<<8)&(BB_ALL ^ (m_allpieces[m_turn] | m_allpieces[oturn])),0);
		//Get 2 moves
		if (noncap)
			interp_pawn_bitboard(moves,(m_pbbs[BB_PAWNS][m_turn]<<16 & BB_3RDRANK)&(BB_ALL ^ (m_allpieces[m_turn] | m_allpieces[oturn])),1);
		//Get Left Captures
		interp_pawn_bitboard(moves,((m_pbbs[BB_PAWNS][m_turn]<<7 & ~BB_RIGHTFILE)&(BB_ALL ^ m_allpieces[m_turn])&m_allpieces[oturn]),2);
		//Get Right Captures
		interp_pawn_bitboard(moves,((m_pbbs[BB_PAWNS][m_turn]<<9 & ~BB_LEFTFILE)&(BB_ALL ^ m_allpieces[m_turn])&m_allpieces[oturn]),3);
	
		//En Passant
		if (m_enpawn > 0 && IS_SET(m_pbbs[BB_PAWNS][oturn],m_enpawn)) {;
			MOVE_SETTO(move,m_enpawn+8);
			if (FILE(m_enpawn)<7 && IS_SET(m_pbbs[BB_PAWNS][m_turn],m_enpawn+1)) {
				MOVE_SETFROM(move,m_enpawn+1);
				moves.insert(move);
			}
			if (FILE(m_enpawn)>0 && IS_SET(m_pbbs[BB_PAWNS][m_turn],m_enpawn-1)) {
				MOVE_SETFROM(move,m_enpawn-1);
				moves.insert(move);
			}
		}
	}
	else {
		//Get 1 moves	
		if (noncap)
			interp_pawn_bitboard(moves,(m_pbbs[BB_PAWNS][m_turn]>>8)&(BB_ALL ^ (m_allpieces[m_turn] | m_allpieces[oturn])),0);
		//Get 2 moves
		if (noncap)
			interp_pawn_bitboard(moves,(m_pbbs[BB_PAWNS][m_turn]>>16 & BB_4THRANK)&(BB_ALL ^ (m_allpieces[m_turn] | m_allpieces[oturn])),1);
		//Get Left Captures
		interp_pawn_bitboard(moves,((m_pbbs[BB_PAWNS][m_turn]>>7 & ~BB_LEFTFILE)&(BB_ALL ^ m_allpieces[m_turn])&m_allpieces[oturn]),2);
		//Get Right Captures
		interp_pawn_bitboard(moves,((m_pbbs[BB_PAWNS][m_turn]>>9 & ~BB_RIGHTFILE)&(BB_ALL ^ m_allpieces[m_turn])&m_allpieces[oturn]),3);
	
		//En Passant
		if (m_enpawn > 0 && IS_SET(m_pbbs[BB_PAWNS][oturn],m_enpawn)) {
			MOVE_SETTO(move,m_enpawn-8);
			if (FILE(m_enpawn)<7 && IS_SET(m_pbbs[BB_PAWNS][m_turn],m_enpawn+1)) {
				MOVE_SETFROM(move,m_enpawn+1);
				moves.insert(move);
			}
			if (FILE(m_enpawn)>0 && IS_SET(m_pbbs[BB_PAWNS][m_turn],m_enpawn-1)) {
				MOVE_SETFROM(move,m_enpawn-1);
				moves.insert(move);
			}
		}
	}
}

// Get pawns striking motion
bitboard Board::get_pawn_attacks() {

	if (m_turn == BB_WHITE) {
		return ((m_pbbs[BB_PAWNS][m_turn]<<7 & ~BB_RIGHTFILE)&(~m_allpieces[m_turn])&m_allpieces[1-m_turn]) |
			((m_pbbs[BB_PAWNS][m_turn]<<9 & ~BB_LEFTFILE)&(~m_allpieces[m_turn])&m_allpieces[1-m_turn]);
	}
	else {
		return ((m_pbbs[BB_PAWNS][m_turn]>>7 & ~BB_LEFTFILE)&(~m_allpieces[m_turn])&m_allpieces[1-m_turn]) |
			((m_pbbs[BB_PAWNS][m_turn]>>9 & ~BB_RIGHTFILE)&(~m_allpieces[m_turn])&m_allpieces[1-m_turn]);
	}
}

// Get the attacks of 1 pawn
bitboard Board::get_single_pawn_attacks(int pos) {
	if (m_turn == BB_WHITE) {
		return (( (m_masks[pos]) <<7 & ~BB_RIGHTFILE)&(~m_allpieces[m_turn])) |
			(( (m_masks[pos])<<9 & ~BB_LEFTFILE)&(~m_allpieces[m_turn]));
	}
	else {
		return (( (m_masks[pos]>>7) & ~BB_LEFTFILE)&(~m_allpieces[m_turn])) |
			(( (m_masks[pos]>>9) & ~BB_RIGHTFILE)&(~m_allpieces[m_turn]));
	}
	
}

bitboard Board::get_single_pawn_attacks_own(int pos) {	
	if (m_turn == BB_WHITE) {
		return (( (get_mask(pos)) <<7 & ~BB_RIGHTFILE)) |
			(( (get_mask(pos))<<9 & ~BB_LEFTFILE));
	}
	else {
		return (( (get_mask(pos)>>7) & ~BB_LEFTFILE)) |
			(( (get_mask(pos)>>9) & ~BB_RIGHTFILE));
	}
}

//Generate the bishop moves
void Board::generate_bishop_moves(MoveList& moves, bool queen, bitboard opp) {
	bitboard bishops; // Get bishop positions
	bitboard atx;
	m_storage from,to;

	(queen) ? bishops = m_pbbs[BB_BISHOPQUEEN][m_turn] : bishops = m_pbbs[BB_BISHOP][m_turn];
	while (bishops) { // Loop through the bishop
		from = BB_NEXTBIT(bishops); // Locate a bishop
		atx = get_bishop_attacks(from) & opp; // Get the attacks from this loc
		while (atx) { // Loop through the set bits in the attack bitboard
			to = BB_NEXTBIT(atx);
			moves.insert(MOVE_FORM_MOVEFTPEXP(0,from,to,BB_BISHOP)); // Make the move
			atx = CLEAR_BIT(atx,to);
		}
		bishops = CLEAR_BIT(bishops,from);
	}	
}

//Get the bishop attacks bitboard
bitboard Board::get_bishop_attacks(int pos) {
	return (((m_diagatxA1H8[pos][(m_allboard[BB_ROTATE_R45R]>>m_R45Rshift[pos]) & m_vrankmasks[m_A1H8length[pos]-1]]) | 
		   (m_diagatxA8H1[pos][(m_allboard[BB_ROTATE_R45L]>>m_R45Lshift[pos]) & m_vrankmasks[m_A8H1length[pos]-1]]))
			 & ~m_allpieces[m_turn]);
}

bitboard Board::get_bishop_attacks_own(int pos) {
	return (((m_diagatxA1H8[pos][(m_allboard[BB_ROTATE_R45R]>>m_R45Rshift[pos]) & m_vrankmasks[m_A1H8length[pos]-1]]) | 
		   (m_diagatxA8H1[pos][(m_allboard[BB_ROTATE_R45L]>>m_R45Lshift[pos]) & m_vrankmasks[m_A8H1length[pos]-1]])));
}

bitboard Board::get_diagA1_attacks_own(int pos) {
	return ((m_diagatxA1H8[pos][(m_allboard[BB_ROTATE_R45R]>>m_R45Rshift[pos]) & m_vrankmasks[m_A1H8length[pos]-1]]));
}

bitboard Board::get_diagH8_attacks_own(int pos) {
	return ((m_diagatxA8H1[pos][(m_allboard[BB_ROTATE_R45L]>>m_R45Lshift[pos]) & m_vrankmasks[m_A8H1length[pos]-1]]));
}
	
void Board::generate_rook_moves(MoveList& moves, bool queen, bitboard opp) {
	bitboard rooks; // Get rook positions
	bitboard atx=BB_ZERO;
	m_storage from,to;

	(queen) ? rooks = m_pbbs[BB_ROOKQUEEN][m_turn] : rooks = m_pbbs[BB_ROOK][m_turn];
	while (rooks) { // Loop through the rooks
		from = BB_NEXTBIT(rooks); // Locate a rook
		atx = get_rook_attacks(from) & opp; // Get the attacks from this loc
		while (atx) { // Loop through the set bits in the attack bitboard
			to = BB_NEXTBIT(atx);
			moves.insert(MOVE_FORM_MOVEFTPEXP(0,from,to,BB_ROOK)); // Make the move
			atx = CLEAR_BIT(atx,to);
		}
		rooks = CLEAR_BIT(rooks,from);
	}
}

//Get the rook attack bitboard
bitboard Board::get_rook_attacks(int pos) {
	return (((m_rankatx[pos][((m_allboard[BB_ROTATE_0]>>(RANK(pos)<<3))) & BB_ROWMASK]) | 
		    (m_fileatx[pos][((m_allboard[BB_ROTATE_R90]>>((7-FILE(pos))<<3))) & BB_ROWMASK])) & ~m_allpieces[m_turn]);
}

bitboard Board::get_rook_attacks_own(int pos) {
		return (((m_rankatx[pos][((m_allboard[BB_ROTATE_0]>>(RANK(pos)<<3))) & BB_ROWMASK]) | 
	    (m_fileatx[pos][((m_allboard[BB_ROTATE_R90]>>((7-FILE(pos))<<3))) & BB_ROWMASK])));
}

bitboard Board::get_rank_attacks_own(int pos) {
	return (m_rankatx[pos][((m_allboard[BB_ROTATE_0]>>(RANK(pos)<<3))) & BB_ROWMASK]);
}

bitboard Board::get_file_attacks_own(int pos) {
	return (m_fileatx[pos][((m_allboard[BB_ROTATE_R90]>>((7-FILE(pos))<<3))) & BB_ROWMASK]);
}

//Obsolete
bitboard Board::rotate_bitboard(bitboard bb, int file) {
	
	int i;
	bitboard bbcol=0;
	// Turn the row bitboard into a file bitboard on right file
	for (i = 0; i < 8; i++) {
		if (IS_SET(bb,i))
			bbcol |= get_mask(ABSOLUTEC(i,file));
	}
	
	return bbcol;
}

//Return a file bitboard aligned in the first 8 bits (1st rank)
bitboard Board::get_column_bitboard(bitboard bb, int file) {
	int i;
	bitboard bbcol=BB_ZERO;
	for (i = 0; i < 8; i++, file += 8) {
		if (IS_SET(bb,ABSOLUTEC(i,file)))
			bbcol |= m_masks[i];
	}
	return bbcol;
}

//Get a pawn column
bitboard Board::get_pawn_column(int color, int file) {
	return (m_pbbs[BB_PAWNS_R90][color] >> ((7-file)<<3)) & BB_ROWMASK;
}

// Generate knight moves
void Board::generate_knight_moves(MoveList& moves, bitboard opp) {
	
	bitboard knights = m_pbbs[BB_KNIGHT][m_turn]; // Get knight positions
	bitboard atx=BB_ZERO;
	m_storage from,to;
	
	while (knights) { // Loop through the knights
		from = BB_NEXTBIT(knights); // Locate a knight
		atx = get_knight_attacks(from) & opp; // Get the attacks from this loc
		while (atx) { // Loop through the set bits in the attack bitboard
			to = BB_NEXTBIT(atx); 
			moves.insert(MOVE_FORM_MOVEFTPEXP(0,from,to,BB_KNIGHT)); // Make the move
			atx = CLEAR_BIT(atx,to);
		}
		knights = CLEAR_BIT(knights,from);
	}
}

// Get knight attacks
bitboard Board::get_knight_attacks(int pos) {
	return m_knightatx[pos] & (BB_ALL ^ m_allpieces[m_turn]);
}

bitboard Board::get_knight_attacks_own(int pos) {
	return m_knightatx[pos];
}

void Board::generate_king_moves(MoveList& moves, bitboard opp, bool noncap) {
	bitboard atx;
	m_storage from,to;

	from = m_kingpos[m_turn]; // Get the king
	if (noncap) generate_castle_moves(moves,from);
	atx = get_king_attacks(from) & opp; // Get the attacks from this loc
	while (atx) { // Loop through the set bits in the attack bitboard
		to = BB_NEXTBIT(atx);
		moves.insert(MOVE_FORM_MOVEFTPEXP(0,from,to,BB_KING)); // Make the move
		atx = CLEAR_BIT(atx,to);
	}
}

bitboard Board::get_king_attacks(int pos) {
	return m_kingatx[pos]&~m_allpieces[m_turn];
}

bitboard Board::get_king_attacks_own(int pos) {
	return m_kingatx[pos];
}

void Board::interp_pawn_bitboard(MoveList& moves, bitboard bb, int type) {

	int i,j,from;

	while (bb) {
		i = BB_NEXTBIT(bb);
		bb = CLEAR_BIT(bb,i);
		switch (type) {	
			case 0:
				// 1 move
				from = (m_turn == BB_WHITE) ? i-8 : i+8;
				break;
			case 1:
				//2 move (assumed that bb is all those pieces that are 2-move rank only)
				if ((m_turn == BB_WHITE && is_occupied(i-8)) ||
					(m_turn == BB_BLACK && is_occupied(i+8)))
						continue;
				from = (m_turn == BB_WHITE) ? i-16 : i+16;
				break;
			case 2:
				//Left Capture
				from = (m_turn == BB_WHITE) ? i-7 : i+7;
				break;
			case 3:
				//Right Capture
				from = (m_turn == BB_WHITE) ? i-9 : i+9;
				break;
		};

		//Promotions
		if (m_turn == BB_WHITE && i >= 56) {
			for (j = BB_KNIGHT; j <= BB_QUEEN; j++) 
				moves.insert(MOVE_FORM_MOVEFULLEXP(0,from,i,BB_PAWNS,j));
		}
		else if (m_turn == BB_BLACK && i < 8) {
			for (j = BB_KNIGHT; j <= BB_QUEEN; j++) 
				moves.insert(MOVE_FORM_MOVEFULLEXP(0,from,i,BB_PAWNS,j));
		}
		else
			moves.insert(MOVE_FORM_MOVEFTPEXP(0,from,i,BB_PAWNS));
	}

}

//Init the king attack bitboards
void Board::init_king_attacks() {
	
	int i,rank,file;
	bitboard allmoves;
	for (i = 0; i < 64; i++) {
		rank = RANK(i); file = FILE(i);
		allmoves = get_safemask(i+1)|get_safemask(i+7)|get_safemask(i+8)|get_safemask(i+9)|
			get_safemask(i-1)|get_safemask(i-7)|get_safemask(i-8)|get_safemask(i-9);
		if (rank == 0) 
			allmoves &= ~(get_safemask(i-7)|get_safemask(i-8)|get_safemask(i-9));
		else if (rank == 7)
			allmoves &= ~(get_safemask(i+7)|get_safemask(i+8)|get_safemask(i+9));
		
		if (file == 0)
			allmoves &= ~(get_safemask(i-9)|get_safemask(i-1)|get_safemask(i+7));
		else if (file == 7)
			allmoves &= ~(get_safemask(i-7)|get_safemask(i+1)|get_safemask(i+9));
		
		m_kingatx[i] = allmoves;
	}
	
}

//Init the bishop attack bitboards
void Board::init_bishop_attacks() {
	int i;
	bitboard j,k;
	
	init_diag_A1H8();
	init_diag_A8H1();

	for (i = 0; i < 64; i++) {
		for (j = BB_ZERO; j < 256; j++) {
			for (k = BB_ZERO; k < 256; k++) {
//				m_bishopatx[i][j][k] = m_diagatxA1H8[i][j] | m_diagatxA8H1[i][k];
			}
		}
	}
}

//Init moves along the A1-H8 diagonal (tricky!)
void Board::init_diag_A1H8() {
	
	bitboard i;
	int pos,j,k,rank,file,count;

	for (pos = 0; pos < 64; pos++) {
		for (i = BB_ZERO; i < BB_ONE<<m_A1H8length[pos]; i++) {
			rank = RANK(pos); file = FILE(pos);
			m_diagatxA1H8[pos][i]= BB_ZERO;
			//Search moving up the right
			count = m_A1H8diagpos[pos]+1;
			for (j = rank+1, k = file+1; j<8 && k<8; j++, k++) {
				if (IS_SET(i,count)) {
					m_diagatxA1H8[pos][i] |= get_mask(ABSOLUTEC(j,k));
					break;
				}
				m_diagatxA1H8[pos][i] |= get_mask(ABSOLUTEC(j,k));
				count++;
			}
			//Search moving up down to the left
			count = m_A1H8diagpos[pos]-1;
			for (j = rank-1, k = file-1; j>=0 && k>=0; j--, k--) {
				if (IS_SET(i,count)) {
					m_diagatxA1H8[pos][i] |= get_mask(ABSOLUTEC(j,k));
					break;
				}
				m_diagatxA1H8[pos][i] |= get_mask(ABSOLUTEC(j,k));
				count--;
			}
		}
	}
}

//Init moves along the A8-H1 diagonal (tricky!)
void Board::init_diag_A8H1() {
	
	bitboard i;
	int pos,j,k,rank,file,count;

	for (pos = 0; pos < 64; pos++) {
		for (i = BB_ZERO; i < BB_ONE<<m_A8H1length[pos]; i++) {
			rank = RANK(pos); file = FILE(pos);
			m_diagatxA8H1[pos][i]= BB_ZERO;
			//Search moving upper left
			count = m_A8H1diagpos[pos]+1;
			for (j = rank+1, k = file-1; j<8 && k>=0; j++, k--) {
				if (IS_SET(i,count)) {
					m_diagatxA8H1[pos][i] |= get_mask(ABSOLUTEC(j,k));
					break;
				}
				m_diagatxA8H1[pos][i] |= get_mask(ABSOLUTEC(j,k));
				count++;
			}
			//Search moving lower right
			count = m_A8H1diagpos[pos]-1;
			for (j = rank-1, k = file+1; j>=0 && k<8; j--, k++) {
				if (IS_SET(i,count)) {
					m_diagatxA8H1[pos][i] |= get_mask(ABSOLUTEC(j,k));
					break;
				}
				m_diagatxA8H1[pos][i] |= get_mask(ABSOLUTEC(j,k));
				count--;
			}
		}
	}
}

//Init hybrid rank/file attacks boards
void Board::init_rook_attacks() {
	int i;
	bitboard j,k;

	init_rank_attacks();
	init_file_attacks();

	for (i = 0; i < 64; i++) {
		for (j = BB_ZERO; j < 256; j++) {
			for (k = BB_ZERO; k < 256; k++) {
//				m_rookatx[i][j][k] = m_rankatx[i][j] | m_fileatx[i][k];
			}
		}
	}
}

//Init the horizontal moves
void Board::init_rank_attacks() {
	int i,k;
	bitboard j;
	for (i = 0; i < 64; i++) {
		// j ranges over all possible rank configurations
		for (j = 0; j < 256; j++) {
			m_rankatx[i][j]=0;
			for (k = FILE(i)+1; k < 8; k++) {
				// When a piece is encountered stop the ray traversal
				if (IS_SET(j,k)) {
					m_rankatx[i][j] |= get_mask(ABSOLUTEC(RANK(i),k));
					break;
				}
				// Square empty implies that this is attackable
				m_rankatx[i][j] |= get_mask(ABSOLUTEC(RANK(i),k));
			}
			for (k = FILE(i)-1; k >= 0; k--) {
				// When a piece is encountered stop the ray traversal
				if (IS_SET(j,k)) {
					m_rankatx[i][j] |= get_mask(ABSOLUTEC(RANK(i),k));
					break;
				}
				// Square empty implies that this is attackable
				m_rankatx[i][j] |= get_mask(ABSOLUTEC(RANK(i),k));
			}
		}
	}
}

//Init the vertical moves
void Board::init_file_attacks() {
	int i,k;
	bitboard j;
	for (i = 0; i < 64; i++) {
		// j ranges over all possible file configurations
		for (j = 0; j < 256; j++) {
			m_fileatx[i][j]=0;
			for (k = RANK(i)+1; k < 8; k++) {
				// When a piece is encountered stop the ray traversal
				if (IS_SET(j,k)) {
					m_fileatx[i][j] |= get_mask(ABSOLUTEC(k,FILE(i)));
					break;
				}
				// Square empty implies that this is attackable
				m_fileatx[i][j] |= get_mask(ABSOLUTEC(k,FILE(i)));
			}
			for (k = RANK(i)-1; k >= 0; k--) {
				// When a piece is encountered stop the ray traversal
				if (IS_SET(j,k)) {
					m_fileatx[i][j] |= get_mask(ABSOLUTEC(k,FILE(i)));
					break;
				}
				// Square empty implies that this is attackable
				m_fileatx[i][j] |= get_mask(ABSOLUTEC(k,FILE(i)));
			}
		}
	}
}

//Init the knight attack boards
void Board::init_knight_attacks() {
	int i,rank,file;
	bitboard allmoves;
	for (i = 0; i < 64; i++) {
		rank = RANK(i); file = FILE(i);
		// All possible knight moves
		allmoves = get_safemask(i+17)|get_safemask(i+10)|get_safemask(i+15)|get_safemask(i+6)|
			get_safemask(i-6)|get_safemask(i-17)|get_safemask(i-10)|get_safemask(i-15);
		// Various bounds filters
		if (rank >= 6) {
			allmoves &=	~(get_safemask(i+17) | get_safemask(i+15));
			if (rank == 7)
				allmoves &= ~(get_safemask(i+10) | get_safemask(i+6));
		}
		if (rank <= 1) {
			allmoves &=	~(get_safemask(i-17) | get_safemask(i-15));
			if (rank == 0)
				allmoves &= ~(get_safemask(i-10) | get_safemask(i-6));
		}
		if (file >= 6) {
			allmoves &= ~(get_safemask(i+10) | get_safemask(i-6));
			if (file == 7)
				allmoves &= ~(get_safemask(i-15) | get_safemask(i+17));
		}
		if (file <= 1) {
			allmoves &= ~(get_safemask(i-10) | get_safemask(i+6));
			if (file == 0)
				allmoves &= ~(get_safemask(i+15) | get_safemask(i-17));
		}
		Board::m_knightatx[i] = allmoves;
	}
}

bitboard Board::get_mask(int pos) {
	return m_masks[pos];
}

bitboard Board::get_safemask(int pos) {
	if (pos < 0) return (bitboard)0;
	else if (pos >= 64) return (bitboard)0;
	else return Board::m_masks[pos];
}
