//Mike Maxim
//Various notational conversion utiliies

#include <notation.h>

string Notation::MoveToSAN(move_t move, Board& board) {

	string str="";
	int fi,fj,ti,tj,prom;
	BoardPosition bp;
	Board sboard;

	if (MOVE_ISCASTLE(move)) 
		str = "O-O";
	else if (MOVE_ISLONGCASTLE(move)) 
		str = "O-O-O";
	else {
		fi = RANK(MOVE_GETFROM(move)); fj = FILE(MOVE_GETFROM(move));
		ti = RANK(MOVE_GETTO(move)); tj = FILE(MOVE_GETTO(move));

		board.get_square(MOVE_GETFROM(move),bp);
		switch (bp.m_type) {
		case BB_KNIGHT:
			str += "N";
			break;
		case BB_BISHOP:
			str += "B";
			break;
		case BB_ROOK:
			str += "R";
			break;
		case BB_QUEEN:
			str += "Q";
			break;
		case BB_KING:
			str += "K";
			break;
		}

		str += clarify(bp.m_type,MOVE_GETFROM(move),MOVE_GETTO(move),board);
		if (board.is_occupied(MOVE_GETTO(move)) || (MOVE_GETPIECE(move) == BB_PAWNS && tj != fj)) {
			if (bp.m_type == BB_PAWNS)
				str += get_file(MOVE_GETFROM(move));
			str += "x";
		}
		str += get_algebra(MOVE_GETTO(move));

		if (MOVE_ISPROMOTION(move)) {
			prom = MOVE_GETPROMOTION(move);
			switch (prom) {
		case BB_KNIGHT:
			str += "=N";
			break;
		case BB_BISHOP:
			str += "=B";
			break;
		case BB_ROOK:
			str += "=R";
			break;
		case BB_QUEEN:
			str += "=Q";
			break;
			};
		}
	}

	sboard = board;
	board.move(move);
	if (board.check()) {
		if (board.winner() != BB_DRAW)
			str += "#";
		else
			str += "+";
	}
	board = sboard;

	return str;

}

move_t Notation::SANtoMove(const string& san, Board& board, bool& success) {

	vector<int> sets;
	m_storage to;
	move_t castle=MOVE_EMPTY,longcastle=MOVE_EMPTY,pmove=MOVE_EMPTY,move=MOVE_EMPTY;
	string ssan = san;
	int prom=0,i;
	Board sboard,ssboard;
	vector<int> psets;

	success = true;
	ssboard = board;
	MOVE_SETCASTLE(castle); MOVE_SETLONGCASTLE(longcastle);
	MOVE_SETPIECE(castle,BB_KING); MOVE_SETPIECE(longcastle,BB_KING);

	if (ssan[ssan.length()-1] == '+' || ssan[ssan.length()-1] == '#')
		ssan = ssan.substr(0,ssan.length()-1);

	if (ssan == "O-O")
		return castle;
	else if (ssan == "O-O-O")
		return longcastle;

	if (ssan.substr(ssan.length()-2,ssan.length()) == "=Q") {
		prom = BB_QUEEN;
		ssan = ssan.substr(0,ssan.length()-2);
	}
	else if (ssan.substr(ssan.length()-2,ssan.length()) == "=R") {
		prom = BB_ROOK;
		ssan = ssan.substr(0,ssan.length()-2);
	}
	else if (ssan.substr(ssan.length()-2,ssan.length()) == "=B") {
		prom = BB_BISHOP;
		ssan = ssan.substr(0,ssan.length()-2);
	}
	else if (ssan.substr(ssan.length()-2,ssan.length()) == "=N") {
		prom = BB_KNIGHT;
		ssan = ssan.substr(0,ssan.length()-2);
	}

	to = from_algebra(ssan.substr(ssan.length()-2,ssan.length()));
	if (to > 64) { board = ssboard; success = false; return MOVE_EMPTY; }

	switch (ssan[0]) {
	case 'K':
		return MOVE_FORM_MOVEFTPEXP(MOVE_EMPTY,board.get_kingpos(board.m_turn),to,BB_KING);
	case 'Q':
		MOVE_SETPIECE(move,BB_QUEEN);
		board.toggleTurn();
		get_setpieces(sets,board.m_pbbs[BB_QUEEN][board.otherTurn()] & (board.get_bishop_attacks(to) | board.get_rook_attacks(to)));
		board.toggleTurn();
		break;
	case 'R':
		MOVE_SETPIECE(move,BB_ROOK);
		board.toggleTurn();
		get_setpieces(sets,board.m_pbbs[BB_ROOK][board.otherTurn()] & board.get_rook_attacks(to));
		board.toggleTurn();
		break;
	case 'B':
		MOVE_SETPIECE(move,BB_BISHOP);
		board.toggleTurn();
		get_setpieces(sets,board.m_pbbs[BB_BISHOP][board.otherTurn()] & board.get_bishop_attacks(to));
		board.toggleTurn();
		break;
	case 'N':
		MOVE_SETPIECE(move,BB_KNIGHT);
		board.toggleTurn();
		get_setpieces(sets,board.m_pbbs[BB_KNIGHT][board.otherTurn()] & board.get_knight_attacks(to));
		board.toggleTurn();
		break;
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
		MOVE_SETTO(pmove,to); MOVE_SETPROMOTION(pmove,prom); MOVE_SETPIECE(pmove,BB_PAWNS);
		if (board.turn() == BB_WHITE) {
			if (strstr(ssan.c_str(),"x")) {
				get_setpieces(sets,(board.m_pbbs[BB_PAWNS][board.m_turn]) & board.get_mask(to-7));
				if (sets.size() > 0 && FILE(to-7) == ssan[0]-'a') {
					MOVE_SETFROM(pmove,to-7); return pmove;
				}
				get_setpieces(sets,(board.m_pbbs[BB_PAWNS][board.m_turn]) & board.get_mask(to-9));
				if (sets.size() > 0 && FILE(to-9) == ssan[0]-'a') {
					MOVE_SETFROM(pmove,to-9); return pmove;
				}
			}
			else {
				get_setpieces(sets,(board.m_pbbs[BB_PAWNS][board.m_turn]) & board.get_mask(to-8));
				if (sets.size() > 0) {
					MOVE_SETFROM(pmove,to-8); return pmove;
				}
				get_setpieces(sets,(board.m_pbbs[BB_PAWNS][board.m_turn]) & board.get_safemask(to-16));
				if (sets.size() > 0) {
					MOVE_SETFROM(pmove,to-16); return pmove;
				}
			}
		}
		else if (board.turn() == BB_BLACK) {
			if (strstr(ssan.c_str(),"x")) {
				get_setpieces(sets,(board.m_pbbs[BB_PAWNS][board.m_turn]) & board.get_mask(to+7));
				if (sets.size() > 0 && FILE(to+7) == ssan[0]-'a') {
					MOVE_SETFROM(pmove,to+7); return pmove;
				}
				get_setpieces(sets,(board.m_pbbs[BB_PAWNS][board.m_turn]) & board.get_mask(to+9));
				if (sets.size() > 0 && FILE(to+9) == ssan[0]-'a') {
					MOVE_SETFROM(pmove,to+9); return pmove;
				}
			}
			else {
				get_setpieces(sets,(board.m_pbbs[BB_PAWNS][board.m_turn]) & board.get_mask(to+8));
				if (sets.size() > 0) {
					MOVE_SETFROM(pmove,to+8); return pmove;
				}
				get_setpieces(sets,(board.m_pbbs[BB_PAWNS][board.m_turn]) & board.get_safemask(to+16));
				if (sets.size() > 0) {
					MOVE_SETFROM(pmove,to+16); return pmove;
				}
			}
		}	
		board = ssboard;
		success = false;
		return MOVE_EMPTY;
	};
	
	//Filter sets
	sboard = board;
	for (i = 0; i < (int)sets.size(); i++) {
		board.move(MOVE_FORM_MOVEFTPEXP(MOVE_EMPTY,sets[i],to,MOVE_GETPIECE(move)));
		board.toggleTurn();
		if (!board.check())
			psets.push_back(sets[i]);
		board.toggleTurn();
		board = sboard;
	}

	board = ssboard;
	if (psets.size() == 1) {
		MOVE_SETFROM(move,psets[0]); MOVE_SETTO(move,to);
		return move;
	}
	else if (psets.size() == 2) {
		if (FILE(psets[0]) != FILE(psets[1])) {
			if (FILE(psets[0]) == ssan[1]-'a') {
				MOVE_SETFROM(move,psets[0]); MOVE_SETTO(move,to);
				return move;
			}
			else if (FILE(psets[1]) == ssan[1]-'a') {
				MOVE_SETFROM(move,psets[1]); MOVE_SETTO(move,to);
				return move;
			}
		}
		else {
			if (RANK(psets[0]) == ssan[1]-'0'-1) {
				MOVE_SETFROM(move,psets[0]); MOVE_SETTO(move,to);
				return move;
			}
			else if (RANK(psets[1]) == ssan[1]-'0'-1) {
				MOVE_SETFROM(move,psets[1]); MOVE_SETTO(move,to);
				return move;
			}
		}
	}
	else if (psets.size() >= 3) {
		if (psets[0] == from_algebra(ssan.substr(1,2))) {
			MOVE_SETFROM(move,psets[0]); MOVE_SETTO(move,to);
			return move;
		}
		else if (psets[1] == from_algebra(ssan.substr(1,2))) {
			MOVE_SETFROM(move,psets[1]); MOVE_SETTO(move,to);
			return move;
		}
		else if (psets[2] == from_algebra(ssan.substr(1,2))) {
			MOVE_SETFROM(move,psets[2]); MOVE_SETTO(move,to);
			return move;
		}
	}

	success = false;
	return MOVE_EMPTY;
}

//Converts a Move object into an Xboard move string
string Notation::MoveToXB(move_t move, Board& board) {

	string str="";
	char conv[2];
	int fi,fj,ti,tj,prom;

	conv[1] = '\0';
	if (MOVE_ISCASTLE(move)) {
		if (board.turn() == BB_BLACK)
			return "e8g8";
		else
			return "e1g1";
	}
	else if (MOVE_ISLONGCASTLE(move)) {
		if (board.turn() == BB_BLACK) 
			return "e8c8";
		else
			return "e1c1";
	}

	fi = RANK(MOVE_GETFROM(move)); fj = FILE(MOVE_GETFROM(move));
	ti = RANK(MOVE_GETTO(move)); tj = FILE(MOVE_GETTO(move));

	conv[0] = 'a'+fj;
	str += conv;
	conv[0] = fi+'0'+1;
	str += conv;
	conv[0] = 'a'+tj;
	str += conv;
	conv[0] = ti+'0'+1;
	str += conv;

	if (MOVE_ISPROMOTION(move)) {
		prom = MOVE_GETPROMOTION(move);
		switch (prom) {
		case BB_KNIGHT:
			str += "n";
			break;
		case BB_BISHOP:
			str += "b";
			break;
		case BB_ROOK:
			str += "r";
			break;
		case BB_QUEEN:
			str += "q";
			break;
		};
	}

	return str;
}

// Converts an Xboard move string into a Move object
move_t Notation::XBtoMove(const string& xmove, Board& board) {
	
	BoardPosition pos;
	move_t move = MOVE_EMPTY;
	int from,to;

	//Is this a valid XB move?
	if (xmove.length() < 4)
		return MOVE_EMPTY;

	if ((xmove[0] < 'a' || xmove[0] > 'h') || (xmove[1] < '1' || xmove[1] > '8') ||
		(xmove[2] < 'a' || xmove[2] > 'h') || (xmove[3] < '1' || xmove[3] > '8'))
		return MOVE_EMPTY;

	from = ABSOLUTEC((int)(xmove[1]-'0'-1),(int)(xmove[0]-'a'));
	to = ABSOLUTEC((int)(xmove[3]-'0'-1),(int)(xmove[2]-'a'));

	MOVE_SETFROM(move,from); MOVE_SETTO(move,to);
	if (from < 0 || from > 63)
		return MOVE_EMPTY;
	if (to < 0 || to > 63)
		return MOVE_EMPTY;

	// Check for castle or long castle
	board.get_square(from,pos);
	MOVE_SETPIECE(move,pos.m_type);
	if (pos.m_type == BB_KING && ABS(from-to) == 2) {
		if (to > from)
			MOVE_SETCASTLE(move);
		else
			MOVE_SETLONGCASTLE(move);
	}

	// Check for promotion
	if (xmove.length() > 4) {
		switch (xmove[4]) {
		case 'n':
			MOVE_SETPROMOTION(move,BB_KNIGHT);
			break;
		case 'b':
			MOVE_SETPROMOTION(move,BB_BISHOP);
			break;
		case 'r':
			MOVE_SETPROMOTION(move,BB_ROOK);
			break;
		case 'q':
			MOVE_SETPROMOTION(move,BB_QUEEN);
			break;
		default:
			return MOVE_EMPTY;
			break;
		};
	}
	
	return move;
}

string Notation::clarify(int type, int from, int to, Board& board) {

	bitboard bb;
	string str = "";
	vector<int> sets;
	int i;

	switch (type) {
	case BB_KNIGHT:
		bb = board.m_pbbs[BB_KNIGHT][board.m_turn] & ~board.get_mask(from);
		get_setpieces(sets,bb);
		for (i = 0; i < (int)sets.size(); i++) {
			if (board.get_knight_attacks(sets[i]) & board.get_mask(to)) {
				if (FILE(sets[i]) == FILE(from)) {
					if (RANK(sets[i]) == RANK(from))
						str += get_algebra(from);
					else
						str += itos(RANK(from)+1);
				}
				else {
					str += get_file(from);
				}
			}
		}
		break;
	case BB_BISHOP:
		bb = board.m_pbbs[BB_BISHOP][board.m_turn] & ~board.get_mask(from);
		get_setpieces(sets,bb);
		for (i = 0; i < (int)sets.size(); i++) {
			if (board.get_bishop_attacks(sets[i]) & board.get_mask(to)) {
				if (FILE(sets[i]) == FILE(from)) {
					if (RANK(sets[i]) == RANK(from))
						str += get_algebra(from);
					else
						str += itos(RANK(from)+1);
				}
				else {
					str += get_file(from);
				}
			}
		}
		break;
	case BB_ROOK:
		bb = board.m_pbbs[BB_ROOK][board.m_turn] & ~board.get_mask(from);
		get_setpieces(sets,bb);
		for (i = 0; i < (int)sets.size(); i++) {
			if (board.get_rook_attacks(sets[i]) & board.get_mask(to)) {
				if (FILE(sets[i]) == FILE(from)) {
					if (RANK(sets[i]) == RANK(from))
						str += get_algebra(from);
					else
						str += itos(RANK(from)+1);
				}
				else {
					str += get_file(from);
				}
			}
		}
		break;
	case BB_QUEEN:
		bb = board.m_pbbs[BB_QUEEN][board.m_turn] & ~board.get_mask(from);
		get_setpieces(sets,bb);
		for (i = 0; i < (int)sets.size(); i++) {
			if ((board.get_rook_attacks(sets[i]) | board.get_bishop_attacks(sets[i]))  & board.get_mask(to)) {
				if (FILE(sets[i]) == FILE(from)) {
					if (RANK(sets[i]) == RANK(from))
						str += get_algebra(from);
					else
						str += itos(RANK(from)+1);
				}
				else {
					str += get_file(from);
				}
			}
		}
		break;
	};
	return str;
}

void Notation::get_setpieces(vector<int>& sets, bitboard bb) {
	int i;
	for (i = 0; i < 64; i++)
		if (IS_SET(bb,i))
			sets.push_back(i);
}

string Notation::get_algebra(int pos) {

	int fi,fj;
	string str="";
	char conv[2];

	fi = RANK(pos); fj = FILE(pos);

	conv[1] = '\0';
	conv[0] = 'a'+fj;
	str += conv;
	conv[0] = fi+'0'+1;
	str += conv;

	return str;
}

string Notation::get_file(int pos) {
	
	int fj;
	string str="";
	char conv[2];

	conv[1]='\0';
	fj = FILE(pos);
	conv[0] = 'a'+fj;
	str += conv;

	return str;
}

int Notation::from_algebra(const string& xmove) {
	return ABSOLUTEC((int)(xmove[1]-'0'-1),(int)(xmove[0]-'a'));
}
