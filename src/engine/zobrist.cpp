//Mike Maxim
//Zobrist setup routines

#include <board.h>

bitboard Board::m_zobrist[6][2][64],Board::m_zobristturn[2],Board::m_zobristcastle[2][2],Board::m_zobristenpawn[65];

unsigned int Board::hash() {
	return (unsigned int)m_zkey;
}

unsigned int Board::pawn_hash() {
	return (unsigned int)m_pzkey;
}

void Board::init_zobrist() {

	int i,j,k;
	srand(BB_ZOBRISTSEED);
	for (i = 0; i < 6; i++) 
		for (j = 0; j < 2; j++) 
			for (k = 0; k < 64; k++)
				m_zobrist[i][j][k] = randbitboard();

	for (i = 0; i < 65; i++)
		m_zobristenpawn[i] = randbitboard();

	m_zobristturn[0] = randbitboard();
	m_zobristturn[1] = randbitboard();
	m_zobristcastle[0][0] = randbitboard(); m_zobristcastle[0][1] = randbitboard(); 
	m_zobristcastle[1][0] = randbitboard(); m_zobristcastle[1][1] = randbitboard();
}

bitboard Board::randbitboard() {
	return (bitboard)rand()^((bitboard)rand() << 15)^((bitboard)rand() << 30)^((bitboard)rand() << 45)^((bitboard)rand() << 60);
}

void Board::zobrist(bitboard& zkey, bitboard& pzkey) const {

	int i,turn;

	zkey = BB_ZERO; pzkey = BB_ZERO;
	// Configuration zobrist
	for (i = 0; i < 64; i++) {
		if (IS_SET(m_allpieces[BB_WHITE],i))
			turn = BB_WHITE;
		else if (IS_SET(m_allpieces[BB_BLACK],i))
			turn = BB_BLACK;
		else continue;

		if (IS_SET(m_pbbs[BB_PAWNS][turn],i)) {
			zkey ^= m_zobrist[0][turn][i];
			pzkey ^= m_zobrist[0][turn][i];
		}
		else if (IS_SET(m_pbbs[BB_KNIGHT][turn],i))
			zkey ^= m_zobrist[1][turn][i];
		else if (IS_SET(m_pbbs[BB_BISHOP][turn],i))
			zkey ^= m_zobrist[2][turn][i];
		else if (IS_SET(m_pbbs[BB_ROOK][turn],i))
			zkey ^= m_zobrist[3][turn][i];
		else if (IS_SET(m_pbbs[BB_QUEEN][turn],i))
			zkey ^= m_zobrist[4][turn][i];
		else if (IS_SET(m_pbbs[BB_KING][turn],i))
			zkey ^= m_zobrist[5][turn][i];
	}

	// Turn zobrist
	if (m_turn == BB_WHITE) zkey ^= m_zobristturn[0];
	else zkey ^= m_zobristturn[1];

	// Castle zobrist
	if (m_castle[0][0]) zkey ^= m_zobristcastle[0][0];
	if (m_castle[0][1]) zkey ^= m_zobristcastle[0][1];
	if (m_castle[1][1]) zkey ^= m_zobristcastle[1][1];
	if (m_castle[1][0]) zkey ^= m_zobristcastle[1][0];

	// En passant pawn zobrist
	zkey ^= m_zobristenpawn[m_enpawn+1];

}
