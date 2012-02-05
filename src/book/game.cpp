//Mike Maxim
//Wrapper for game information

#ifdef WIN32
#pragma warning (disable: 4180)
#pragma warning (disable: 4018)
#pragma warning (disable: 4311)
#endif

#include <game.h>

Game::Game() : m_event("???"), m_eco("???") {
	reset();
	m_names[0] = m_names[1] = "???";
	m_elos[0] = m_elos[1] = 0;
}

void Game::commit(const string& file) {
	
	FILE* ffile;
	Board board;
	string str;
	int i,move=1;

	if (NULL == (ffile = fopen(file.c_str(),"a+")))
		return;
	
	board.newGame();
	write_tags(ffile);
	for (i = 0; i < (int)m_moves.size(); i++) {
		if ((i % 2) == 0) {
			str = itos(move++) + ". ";
			fwrite(str.c_str(),1,str.length(),ffile); 
		}
		str = Notation::MoveToSAN(m_moves[i],board) + " ";
		fwrite(str.c_str(),1,str.length(),ffile);
		board.move(m_moves[i]);
	}
	str = m_result + "\n\n";
	fwrite(str.c_str(),1,str.length(),ffile);

	fclose(ffile);
}

void Game::set_moves(const vector<move_t>& moves) {
	int i;
	m_moves.clear();
	for (i = 0; i < (int)moves.size(); i++)
		m_moves.push_back(moves[i]);
}

void Game::write_tags(FILE* file) {

	char buffer[256];

	sprintf(buffer,"[Event \"%s\"]\n",m_event.c_str());
	fwrite(buffer,1,strlen(buffer),file);
	sprintf(buffer,"[White \"%s\"]\n",m_names[BB_WHITE].c_str());
	fwrite(buffer,1,strlen(buffer),file);
	sprintf(buffer,"[Black \"%s\"]\n",m_names[BB_BLACK].c_str());
	fwrite(buffer,1,strlen(buffer),file);
	sprintf(buffer,"[Result \"%s\"]\n",m_result.c_str());
	fwrite(buffer,1,strlen(buffer),file);
	sprintf(buffer,"[WhiteElo \"%d\"]\n",m_elos[BB_WHITE]);
	fwrite(buffer,1,strlen(buffer),file);
	sprintf(buffer,"[BlackElo \"%d\"]\n",m_elos[BB_BLACK]);
	fwrite(buffer,1,strlen(buffer),file);
	sprintf(buffer,"[ECO \"%s\"]\n\n",m_eco.c_str());
	fwrite(buffer,1,strlen(buffer),file);
}

void Game::reset() {
	m_board.newGame();
	m_mindex = 0;
}

bool Game::next_position() {
	if (m_mindex >= (int)m_moves.size()) return false;

	m_board.move(m_moves[m_mindex++]);
	return true;
}

move_t Game::get_nextmove() {
	return m_moves[m_mindex];
}

Board& Game::get_position() {
	return m_board;
}

void Game::get_moves(vector<move_t>& moves) {

	int i;
	for (i = 0; i < (int)m_moves.size(); i++)
		moves.push_back(m_moves[i]);
}

void Game::setEvent(const string& event) {
	m_event = event;
}

string Game::getEvent() const {
	return m_event;
}

void Game::setECO(const string& eco) {
	m_eco = eco;
}

string Game::getECO() const {
	return m_eco;
}

void Game::setElo(int color, int elo) {
	m_elos[color]=elo;
}

int Game::getElo(int color) const {
	return m_elos[color];
}

void Game::setName(int color, const string& name) {
	m_names[color] = name;
}

string Game::getName(int color) const {
	return m_names[color];
}

void Game::setResult(const string& result) {
	m_result = result;
}

string Game::getResult() const {
	return m_result;
}

bool Game::isBlank() const {
    return (m_moves.size() == 0);
}   

void Game::replay_game() {

	Board board;
	vector<move_t>::iterator i;

	board.newGame();
	for (i = m_moves.begin(); i != m_moves.end(); i++) {
		cout << board.toString() << endl;
		cout << "Return for next move..."; getchar();
		cout << "Move: " << Notation::MoveToSAN(*i,board) << endl;
		board.move(*i);
	}
	cout << board.toString() << endl << m_result << endl << endl;
}

// Game is given with highly filtered PGN format
// No comments, no locale info, just the moves.
void Game::parse_game(const string& game) {

	Board board;
	move_t move;
	string fgame,ntoken;
	bool success;
	int i;

	fgame = filter_numbers(game);

	board.newGame();
	StringTokenizer moves(fgame," ");
	m_moves.clear();
	for (i = 0; i < (int)moves.size(); i++) {

		ntoken = moves[i];
		if (ntoken == "1-0" || ntoken == "0-1" || ntoken == "1/2-1/2" ||
            ntoken == "*") {
			m_result = ntoken;
			continue;
		}

		move = Notation::SANtoMove(ntoken,board,success);
		if (MOVE_ISBLANKMOVE(move)) {
			m_result = "1/2-1/2";
			cout << "ERROR: BAD MOVE: " << ntoken << endl;
			return;
		}
		m_moves.push_back(move);
		board.move(move);
	}
    if (m_moves.size() == 0)
        cout << "WARNING: Blank game!" << endl;
}

string Game::filter_numbers(const string& game) {

	StringTokenizer tokens(game," ");
	string fgame,ctoken;
	int i;

	for (i = 0; i < (int)tokens.size(); i++) {
		ctoken = tokens[i];
		if (ctoken[0] >= '0' && ctoken[0] <= '9' && ctoken[ctoken.length()-1] == '.')
			continue;
		fgame += ctoken + " ";
	}

	return fgame;
}
