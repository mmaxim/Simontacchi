//Mike Maxim
//Book generator

#include <bookgen.h>

BookGenerator::BookGenerator() {

}

void BookGenerator::create_book(const string& file, const string& outfile, int ply) {

	vector<Game> games;
	move_t move;
	BKBoardRecord brec;
	bitboard position;
	int i,j;
	bool* dones=NULL;

	cout << "BOOKMAKE: Loading Game File" << endl;
	m_pgnload.load(file);
	games = m_pgnload.get_games();

	dones = new bool[games.size()];
	for (i = 0; i < (int)games.size(); i++) dones[i]=false;

	cout << "BOOKMAKE: Creating Book" << endl;
	for (i = 0; i < ply; i++) {
		for (j = 0; j < (int)games.size(); j++) {
			
			// Is game over?
			if (dones[j])
				continue;
            
            // Is blanks?
            if (games[j].isBlank())
                continue;

			move = games[j].get_nextmove();
			position = games[j].get_position().get_signature();
			//Retrieve Record for this position
			if (m_bmap.find(position) == m_bmap.end()) {
				brec.m_board = position; brec.m_total = 0;
				m_bmap.insert(strboardmap::value_type(position,brec));
			}
			
			// Augment move list
			m_bmap[position].addMove(move);
			m_bmap[position].m_total++;
			
			// Next move
			dones[j] = !games[j].next_position();
		}
	}
	
	if (dones != NULL) {
		delete [] dones;
		dones = NULL;
	}

	cout << "BOOKMAKE: Committing book to disk" << endl; 
	write_to_file_bin(outfile);
	cout << "BOOKMAKE: Done." << endl;
}

void BookGenerator::write_to_file_bin(const string& outfile) {

	int i,data;
	strboardmap::iterator it;
	FILE* ofile;
	Board cboard;
	BKMoveRecord bmr;

	ofile = fopen(outfile.c_str(),"wb");
	data = BOOK_MAGICNUM; fwrite(&data,sizeof(int),1,ofile);
	for (it = m_bmap.begin(); it != m_bmap.end(); it++) {
		data = 0; fwrite(&data,sizeof(int),1,ofile);
		fwrite(&it->second.m_board,sizeof(bitboard),1,ofile);

		data = (int)it->second.m_moves.size(); fwrite(&data,sizeof(int),1,ofile);
		data = it->second.m_total; fwrite(&data,sizeof(int),1,ofile);
		for (i = 0; i < (int)it->second.m_moves.size(); i++) {
			bmr = it->second.m_moves[i];
			fwrite(&bmr,sizeof(BKMoveRecord),1,ofile);
		}
	}
	data = -1; fwrite(&data,sizeof(int),1,ofile);
	fclose(ofile);
}

int BookGenerator::convert(char code) {

	if (code >= '0' && code <= '8')
		return code-'0';

	switch (code) {
	case 'K': return 9;
	case 'Q': return 10;
	case 'R': return 11;
	case 'B': return 12;
	case 'N': return 13;
	case 'P': return 14;
	case 'k': return 15;
	case 'q': return 16;
	case 'r': return 17;
	case 'b': return 18;
	case 'n': return 19;
	case 'p': return 20;
	case '/': return 21;
	case ' ': return 22;
	case '-': return 23;
	case 'w': return 24;
	case 'a': return 25;
	case 'c': return 26;
	case 'd': return 27;
	case 'e': return 28;
	case 'f': return 29;
	case 'g': return 30;
	case 'h': return 31;
	};

	return BOOK_ENDPOS;
}

void BookGenerator::write_to_file_txt(const string& outfile) {

/*	int i;
	strboardmap::iterator it;
	string str;
	FILE* ofile;
	BKMoveRecord bmr;
	Board cboard;

	ofile = fopen(outfile.c_str(),"w");
	for (it = m_bmap.begin(); it != m_bmap.end(); it++) {
		fwrite((it->second.m_board + "\n").c_str(),1,(it->second.m_board + "\n").length(),ofile);
		str = itos((int)it->second.m_moves.size()) + " " + itos(it->second.m_total) + "\n";
		fwrite(str.c_str(),1,str.length(),ofile);
		cboard.fromFEN(it->second.m_board);
		for (i = 0; i < (int)it->second.m_moves.size(); i++) {
			bmr = it->second.m_moves[i];
			str = Notation::MoveToXB(bmr.m_move,cboard) + " " + itos(bmr.m_freq) + "\n";
			fwrite(str.c_str(),1,str.length(),ofile);
		}
	}
	fclose(ofile);*/
}

void BKBoardRecord::addMove(move_t move) {
	int i;
	for (i = 0; i < (int)m_moves.size(); i++) {
		if (m_moves[i].m_move == move) {
			m_moves[i].m_freq++; return;
		}
	}
	m_moves.push_back(BKMoveRecord(move,1));
}

string BKBoardRecord::toString() {
	Board rboard;
	string str="Obsolete";

	return str;
}

string BKBoardRecord::toThinkString(Board& board) {
	
	string str="( ";
	int i,perc;

	for (i = 0; i < (int)m_moves.size(); i++) {
		perc = (int)(100.0*((double(m_moves[i].m_freq)/double(m_total))));
		if (perc > 10) {
			str += Notation::MoveToSAN(m_moves[i].m_move,board) + "(" + itos(perc) + "%) ";
		}
	}
	str += ")";

	return str;
}

string BKMoveRecord::toString() {
	return "(F: " + itos(m_freq) + " M: " + itos(m_move) + ")";
}
