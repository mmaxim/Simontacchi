//Mike Maxim
//Book implementation

#include <book.h>

Book::Book() {
	srand((unsigned int)time(NULL));
}

//Find a book move
bool Book::book_move(Board& board, move_t& move) {

	bitboard sig = board.get_signature();
	srand((unsigned int)time(NULL));
	if (m_book.find(sig) != m_book.end()) {
		move = get_probmove(m_book[sig]);
		return true;
	}

	return false;
}

//Find most likely
move_t Book::get_probmove(const BKBoardRecord& brec) {
	
	int i,bindex=0,bscore=0,score;
	for (i = 0; i < (int)brec.m_moves.size(); i++) {
		score = 0+rand()%brec.m_moves[i].m_freq;	
		if (score > bscore) {
			bscore = score;
			bindex = i;
		}
	}

	return brec.m_moves[bindex].m_move;
}

//Get all bookmoves for a given position
bool Book::get_bookmoves(Board& board, BKBoardRecord& rec) {

	bitboard sig = board.get_signature();
	if (m_book.find(sig) != m_book.end()) {
		rec = m_book[sig];
		return true;
	}

	return false;
}

int Book::get_positions() {
	return (int)m_book.size();
}

//Load book
bool Book::open_book(const string& book) {

	//BufferedReader reader;
	FILE* ifile;
	bitboard position;
	int data;

	if (NULL == (ifile = fopen(book.c_str(), "rb"))) {
        WRITEPIPE("BOOK: Cannot file book file\n");
		return false;
    }

	fread(&data,sizeof(int),1,ifile);
	//reader.read((char*)&data, sizeof(int));
	if (data != BOOK_MAGICNUM)
		return false;

	while (true) {
		fread(&data,sizeof(int),1,ifile);
//		reader.read((char*)&data, sizeof(int));
//		cout << data << " " ;
		if (data == -1) break;

		fread(&position,sizeof(bitboard),1,ifile);
//		reader.read((char*)&position, sizeof(bitboard));
		proc_position(position, ifile);
	}

	fclose(ifile);
//	reader.close();

	return true;
}

//Close book
void Book::close_book() {
	m_book.clear();	
}

void Book::proc_position(bitboard position, FILE* file) {

	BKBoardRecord brec;
	BKMoveRecord mrec;
	string line;
	int moves,total,i;
	Board cboard;

	if (m_book.find(position) == m_book.end()) {
		brec.m_board = position; brec.m_total = 0;
		m_book.insert(strboardmap::value_type(position,brec));
	}

	//reader.read((char*)&moves, sizeof(int));
	//reader.read((char*)&total, sizeof(int));
	fread(&moves,sizeof(int),1,file);
	fread(&total,sizeof(int),1,file);

	m_book[position].m_total = total;
	for (i = 0; i < moves; i++) {
		fread(&mrec, sizeof(BKMoveRecord), 1, file);
//		reader.read((char*)&mrec, sizeof(BKMoveRecord));
		m_book[position].m_moves.push_back(mrec);
	}
}
