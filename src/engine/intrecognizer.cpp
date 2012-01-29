//Mike Maxim
//Interior recognizer implementation

#include <intrecognizer.h>

InteriorRecognizer::InteriorRecognizer(int drawscore) : m_drawscore(drawscore) {

}

DWORD InteriorRecognizer::probe(Board& board, int& score) {

	int i,index;

	//Check for known drawn conditions
	//Check for 3-rep draw
	if (is_repeated(board)) {
		score = m_drawscore;
		return INTRECOG_MAKEDRAW(INTRECOG_DRAW_REP);
	}

	//Check for lack of material draw
	if ((board.m_pbbs[BB_PAWNS][BB_WHITE] == BB_ZERO && board.m_pbbs[BB_PAWNS][BB_BLACK] == BB_ZERO) && 
		(board.m_pbbs[BB_ROOK][BB_WHITE] == BB_ZERO  && board.m_pbbs[BB_ROOK][BB_BLACK] == BB_ZERO) &&
		(board.m_pbbs[BB_QUEEN][BB_WHITE] == BB_ZERO && board.m_pbbs[BB_QUEEN][BB_BLACK] == BB_ZERO)) {
		if (board.get_material(BB_WHITE) <= 0 &&
			(Board::bb_count(board.m_pbbs[BB_BISHOP][BB_BLACK]) + Board::bb_count(board.m_pbbs[BB_KNIGHT][BB_BLACK]) <= 1)) {
			score = m_drawscore;
			return INTRECOG_MAKEDRAW(INTRECOG_DRAW_MAT);
		}
		else if (board.get_material(BB_BLACK) <= 0 &&
			(Board::bb_count(board.m_pbbs[BB_BISHOP][BB_WHITE]) + Board::bb_count(board.m_pbbs[BB_KNIGHT][BB_WHITE]) <= 1)) {
			score = m_drawscore;
			return INTRECOG_MAKEDRAW(INTRECOG_DRAW_MAT);
		}
	}

	//Check for 50 move rule draw
	for (i = (int)m_gamehist.size()-1, index=0; i >= 0; i--) {
		
		if (m_gamehist[i].m_capture || m_gamehist[i].m_pawnmove)
			break;

		if (++index >= 50) {
			score = m_drawscore;
			return INTRECOG_MAKEDRAW(INTRECOG_DRAW_50);
		}
	}

	//Probe EGTBs
	//void egtb_probe();

	return INTRECOG_UNKNOWN;
}

//Reset all player game history information
void InteriorRecognizer::reset() {
	m_gamehist.clear(); clear_reptable();
}

void InteriorRecognizer::push_game_position(Board& board, move_t move) {
	HistEntry ent;

	ent.m_board = board; ent.m_move = move;
	ent.m_pawnmove = (MOVE_GETPIECE(move) == BB_PAWNS);
	ent.m_capture = board.is_occupied(MOVE_GETTO(move));

	m_gamehist.push_back(ent);
	clear_reptable();
}

void InteriorRecognizer::pop_game_position() {
	m_gamehist.pop_back();
	clear_reptable();
}

void InteriorRecognizer::add_search_position(Board& board) {
	update_reptable(board);
}

void InteriorRecognizer::remove_search_position(Board& board) {
	remove_reptable(board);
}

void InteriorRecognizer::update_reptable(Board& board) {

	int hvalue = board.hash() % REPTABLE_SIZE;

	m_reptable[hvalue].push_back(board.get_signature());
}

void InteriorRecognizer::remove_reptable(Board& board) {

	int hvalue = board.hash() % REPTABLE_SIZE;
	vector<bitboard>::iterator i;
	
	bitboard sig = board.get_signature();
	//Loop though the s chaining list
	for (i = m_reptable[hvalue].begin(); i != m_reptable[hvalue].end(); i++) {
		if (*i == sig) {
			m_reptable[hvalue].erase(i);
			return;
		}
	}
}

bool InteriorRecognizer::is_repeated(Board& board) {

	int hvalue = board.hash() % REPTABLE_SIZE;
	vector<bitboard>::iterator i;

	bitboard sig = board.get_signature();
	//Loop though the s chaining list
	for (i = m_reptable[hvalue].begin(); i != m_reptable[hvalue].end(); i++) {
		if (*i == sig)
			return true;
	}
	return false;
}

void InteriorRecognizer::clear_reptable() {

	int i,j,added=0;
	Board curpos;

	//First zero everything
	for (i = 0; i < REPTABLE_SIZE; i++)
		m_reptable[i].clear();

	// Fill the repetition table with those boards which occur twice in game history
	for (i = 0; i < (int)m_gamehist.size(); i++) {
		curpos = m_gamehist[i].m_board;	
		for (j = 0; j < (int)m_gamehist.size(); j++) {
			if (i == j) continue;
			if (m_gamehist[j].m_board == curpos) {
				update_reptable(curpos);
				break;
			}
		}
	}
}

bool InteriorRecognizer::is_reptable_empty() {
	int i;
	for (i = 0; i < REPTABLE_SIZE; i++)
		if (m_reptable[i].size() != 0)
			return false;
	return true;
}