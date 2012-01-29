//Mike Maxim
//FEN conversion utilities

#include <board.h>
#include <notation.h>

string Board::toFEN(bool moves) const {

	string fenstr;
	
	fenstr = fen_buildpieces() + " ";
	if (m_turn == BB_WHITE)
		fenstr += "w ";
	else
		fenstr += "b ";

	if (!m_castle[BB_WHITE][CASTLE_SHORT] && !m_castle[BB_WHITE][CASTLE_LONG] &&
		!m_castle[BB_BLACK][CASTLE_SHORT] && !m_castle[BB_BLACK][CASTLE_LONG])
		fenstr += "-";
	else {
		if (m_castle[BB_WHITE][CASTLE_SHORT])
			fenstr += "K";
		if (m_castle[BB_WHITE][CASTLE_LONG])
			fenstr += "Q";
		if (m_castle[BB_BLACK][CASTLE_SHORT])
			fenstr += "k";
		if (m_castle[BB_BLACK][CASTLE_LONG])
			fenstr += "q";
	}

	fenstr += " ";
	if (m_enpawn > 0)
		fenstr += Notation::get_algebra(m_enpawn) + " ";
	else
		fenstr += "- ";

	if (moves) {
		fenstr += itos(m_nummoves*2) + " " + itos(m_nummoves);
	}
	else {
		fenstr += "0 0";
	}

	return fenstr;
}

string Board::fen_buildpieces() const {
	
	BoardPosition bp;
	int rank,file,empty=0;
	string str="";

	for (rank = 7; rank >= 0; rank--) {
		for (file = 0; file < 8; file++) {
			get_square(ABSOLUTEC(rank,file),bp);

			if (bp.m_color == BB_DRAW) {
				empty++; continue;
			}
			else if (empty > 0) {
				str += itos(empty); empty = 0;
			}

			switch (bp.m_type) {
			case BB_PAWNS:
				str += (bp.m_color == BB_WHITE) ? "P" : "p";
				break;
			case BB_KNIGHT:
				str += (bp.m_color == BB_WHITE) ? "N" : "n";
				break;
			case BB_BISHOP:
				str += (bp.m_color == BB_WHITE) ? "B" : "b";
				break;
			case BB_ROOK:
				str += (bp.m_color == BB_WHITE) ? "R" : "r";
				break;
			case BB_QUEEN:
				str += (bp.m_color == BB_WHITE) ? 'Q' : 'q';
				break;
			case BB_KING:
				str += (bp.m_color == BB_WHITE) ? 'K' : 'k';
				break;
			};
		}
		if (empty > 0)
			str += itos(empty);
		
		if (rank > 0)
			str += "/";
		
		empty = 0;
	}

	return str;
}

bool Board::fromFEN(const string& fen) {

	StringTokenizer tok(fen," ");
	string castlestr,enpass;
	int i;

	init();

	if (tok.size() == 0)
		return false;

	fen_parsepieces(tok[0]);
	if (tok.size() == 1) return false;
	if (tok[1] == "w")
		m_turn = BB_WHITE;
	else
		m_turn = BB_BLACK;

	if (tok.size() == 2) return false;
	castlestr = tok[2];
	m_castle[0][0] = m_castle[0][1] = m_castle[1][0] = m_castle[1][1] = false;
	for (i = 0; i < (int)castlestr.length(); i++) {
		if (castlestr[i] == 'K')
			m_castle[BB_WHITE][CASTLE_SHORT]=true;
		else if (castlestr[i] == 'Q')
			m_castle[BB_WHITE][CASTLE_LONG]=true;
		else if (castlestr[i] == 'k')
			m_castle[BB_BLACK][CASTLE_SHORT]=true;
		else if (castlestr[i] == 'q')
			m_castle[BB_BLACK][CASTLE_LONG]=true;
		else if (castlestr[i] != '-')
			return false;
	}

	m_enpawn = -1;
	if (tok.size() == 3) return false;
	if ((enpass = tok[3]) != "-") 
		m_enpawn = ABSOLUTEC((int)(enpass[1]-'0'-1),(int)(enpass[0]-'a'));
	
	if (tok.size() > 4) {
		if (tok.size() == 5) return false;
		m_nummoves = atoi(tok[5].c_str());
	}
	else 
		m_nummoves = 1;

	finalizeSetup();
	return true;
}

void Board::fen_parsepieces(const string& pieces) {

	int i,rank,file,sqr,turn;
	char curc;
	rank = 7; file = 0;
	for (i = 0; i < (int)pieces.length(); i++) {
		curc = pieces[i];
		if (curc >= '0' && curc <= '9') {
			file += curc-'0';
			if (file == 8) {
				file = 0; rank--;
			}
			continue;
		}
		if (curc == '/')
			continue;

		if (curc >= 'a' && curc <= 'z') {
			turn = BB_BLACK;
			curc = toupper(curc);
		}
		else
			turn = BB_WHITE;

		sqr = ABSOLUTEC(rank,file);
		switch (curc) {
		case 'P':
			m_allpieces[turn] |= get_mask(sqr);
			m_pbbs[BB_PAWNS][turn] |= get_mask(sqr);
			break;
		case 'N':
			m_allpieces[turn] |= get_mask(sqr);
			m_pbbs[BB_KNIGHT][turn] |= get_mask(sqr);
			break;
		case 'B':
			m_allpieces[turn] |= get_mask(sqr);
			m_pbbs[BB_BISHOP][turn] |= get_mask(sqr);
			break;
		case 'R':
			m_allpieces[turn] |= get_mask(sqr);
			m_pbbs[BB_ROOK][turn] |= get_mask(sqr);
			break;
		case 'Q':
			m_allpieces[turn] |= get_mask(sqr);
			m_pbbs[BB_QUEEN][turn] |= get_mask(sqr);
			break;
		case 'K':
			m_allpieces[turn] |= get_mask(sqr);
			m_pbbs[BB_KING][turn] |= get_mask(sqr); m_kingpos[turn] = sqr;
			break;
		}
		file++;
		if (file > 7) {
			rank--; file = 0;
		}
	}
}
