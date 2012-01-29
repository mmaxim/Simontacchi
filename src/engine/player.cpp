//Mike Maxim
//Main engine code repository

#include <player.h>
#include <engine.h>

Book* Player::m_book = NULL;

Player::Player() : m_eval(NULL), m_recog(NULL) {
}

Player::~Player() {
	if (m_transtable != NULL) { delete [] m_transtable; m_transtable = NULL; }
	if (m_eval != NULL) { delete m_eval; m_eval = NULL; }
	if (m_recog != NULL) { delete m_recog; m_recog = NULL; }
}

//Initialize the player module
void Player::init(const PlayerInit& idata) {

	//Setup options
	m_options[PLAYER_SHOWTHINKING]=idata.m_showthinking;
	m_options[PLAYER_TRANSSIZE]=idata.m_transsize;
	m_options[PLAYER_TRANSENABLED]=idata.m_transenabled;
	m_options[PLAYER_USETIMING]=idata.m_usetiming;
	m_options[PLAYER_NULLDEPTH]=PLAYER_PLY_FULL(idata.m_nulldepth);
	m_options[PLAYER_BOOKUSAGE]=idata.m_usebook;
	m_options[PLAYER_DISPLAY]=idata.m_display;
	m_options[PLAYER_IDSTART]=idata.m_idstart;
	m_options[PLAYER_CONTEMPT]=idata.m_contempt;
	m_options[PLAYER_PVSENABLED]=idata.m_pvs;
	m_options[PLAYER_PTRANSSIZE]=idata.m_ptranssize;
	m_options[PLAYER_EVALCACHESIZE]=idata.m_evalcachesize;
	m_options[PLAYER_LAZYEVAL]=idata.m_lazy;
	m_options[PLAYER_LAZYWINDOW]=idata.m_lazywindow;
	m_options[PLAYER_ASPIRATION]=idata.m_asp;
	m_options[PLAYER_ASP_WINDOW]=idata.m_abwindow;
	m_options[PLAYER_EXTFUTILE]=idata.m_extfutile;
	m_options[PLAYER_RAZOR]=idata.m_razor;
	m_options[PLAYER_NULLENABLED]=idata.m_nullenabled;

	//Create evaluator
	m_eval = new Evaluator(idata.m_lazy,idata.m_lazywindow);

	//Create interior recognizer
	m_recog = new InteriorRecognizer(idata.m_contempt);

	//Allocate main transposition table
	if (m_transtable == NULL) {
		m_transtable = new TTEntry[m_options[PLAYER_TRANSSIZE]];
	}
		
	//Clear entries and set up aux tables
	clear_trans();

	m_oldscore = 0;
	
	//Open the opening book
	if (m_book == NULL) {
		m_book = new Book();
		if (!m_book->open_book(idata.m_bookfile)) {
			cout << "PLAYER: Error loading book file. Opening Book Off." << endl;
			m_options[PLAYER_BOOKUSAGE]=0;
		}
	}

	//Init randomness (Perl::Entanglement?? ;))
	srand((unsigned int)time(NULL));
}

void Player::clear_trans() {
	
	int i;

	//Clear all main TT entries
	for (i = 0; i < m_options[PLAYER_TRANSSIZE]; i++) {
		m_transtable[i].m_info.m_depth = -1;
		m_transtable[i].m_sig = BB_ZERO;
		m_transtable[i].m_info.m_move = MOVE_EMPTY;
		m_transtable[i].m_pvroot = false;
	}
	
	//Set up evaluator pawn tables
	Evaluator::set_pawnsize(m_options[PLAYER_PTRANSSIZE]);
	Evaluator::set_evalsize(m_options[PLAYER_EVALCACHESIZE]);
}

void Player::toggleOption(int option, int set) {
	m_options[option] = set;

	if (option == PLAYER_TRANSSIZE) {
		delete [] m_transtable; m_transtable = NULL;
		m_transtable = new TTEntry[m_options[PLAYER_TRANSSIZE]];
	}
	else if (option == PLAYER_PTRANSSIZE) 
		Evaluator::set_pawnsize(m_options[PLAYER_PTRANSSIZE]);
	else if (option == PLAYER_EVALCACHESIZE) 
		Evaluator::set_evalsize(m_options[PLAYER_EVALCACHESIZE]);
}

void Player::show_config() {
	
	cout << "PLAYER: Transposition Entries: " << m_options[PLAYER_TRANSSIZE] << endl;
	cout << "PLAYER: Null Enabled: " << m_options[PLAYER_NULLENABLED] << endl;
	cout << "PLAYER: Null Depth: " << PLAYER_PLY_LOGICAL(m_options[PLAYER_NULLDEPTH]) << endl;
	cout << "PLAYER: Pawn Trans Entries: " << m_options[PLAYER_PTRANSSIZE] << endl;
	cout << "PLAYER: Eval Cache Entries: " << m_options[PLAYER_EVALCACHESIZE] << endl;
	cout << "PLAYER: Lazy Eval Enabled: " << m_options[PLAYER_LAZYEVAL] << endl;
	cout << "PLAYER: Lazy Window: " << m_options[PLAYER_LAZYWINDOW] << endl;
	cout << "PLAYER: Aspiration Enabled: " << m_options[PLAYER_ASPIRATION] << endl;
	cout << "PLAYER: Aspiration Window: " << m_options[PLAYER_ASP_WINDOW] << endl;
	cout << "PLAYER: Extended Futility Window: " << m_options[PLAYER_EXTFUTILE] << endl;
	cout << "PLAYER: Razor Point: " << m_options[PLAYER_RAZOR] << endl;
	cout << "PLAYER: Contempt Factor: " << m_options[PLAYER_CONTEMPT] << endl;
	cout << "PLAYER: Iterative Deepening: " << m_options[PLAYER_IDSTART] << endl;

}

void Player::change_books(const string& book) {

	m_book->close_book();

	if (!m_book->open_book(book)) {
		cout << "PLAYER: Error loading book file. Opening Book Off." << endl;
		m_options[PLAYER_BOOKUSAGE]=0;
	}
}

int Player::getOption(int opt) {
	return m_options[opt];
}

//Give the user a hint
move_t Player::get_hint() {
	if (m_oldline.m_length > 1)
		return m_oldline.m_moves[1];
	return MOVE_EMPTY;
}

//Initiate the move though process
int Player::get_move(Board& board, move_t& move, double timeleft) {

	m_tnodes=0;
	BKBoardRecord brec;
	if (m_options[PLAYER_BOOKUSAGE] && m_book->book_move(board,move)) {
		if (m_options[PLAYER_SHOWTHINKING]) {
			m_book->get_bookmoves(board,brec);
			WRITEPIPE(string("0 0 0 0 " + brec.toThinkString(board) + "\n").c_str());
			WRITEPIPE("\n");
		}
		return 0;
	}
	else {
		m_timeallowed = timeleft;
		return search(board,move);
	}
}

//Force a move
void Player::move_now() {
	m_movenow = true;
}

//Change the time left
void Player::modify_timeleft(double timeleft) {
	m_timeallowed = timeleft;
	m_starttime = getMS();
	if (m_options[PLAYER_SHOWTHINKING])
		WRITEPIPE(string(string("\n") + "STATUS: ALLOCATED: " + dtos(m_timeallowed) + " MS\n").c_str());
}

Book* Player::get_book() {
	return m_book;
}

//Perform the iterative deepening process
int Player::search(Board& sboard, move_t& move) {

	int i,j,k,bscore=0;
	int score=0,alpha,beta;
	bool spoken=false;
	MoveList moves;
	GameTree tree;
	DisplayStats stats;
	Board board;

	board = sboard;
	m_starttime = m_lastnode = getMS();
	m_movenow = false;
	srand((unsigned int)time(NULL));

	if (m_options[PLAYER_SHOWTHINKING] && m_options[PLAYER_DISPLAY] != DISPLAY_MANAGER_UCI) {
		if (m_timeallowed >= 0)
			WRITEPIPE(string(string("\n") + "STATUS: ALLOCATED: " + dtos(m_timeallowed) + " MS\n").c_str());
		else
			WRITEPIPE(string(string("\n") + "STATUS: ALLOCATED: Infinite\n").c_str());

		DisplayManager::getInstance(m_options[PLAYER_DISPLAY])->output_startsearch();
	}

	// History Heuristic init
	for (i = 0; i < 66; i++) for (j = 0; j < 66; j++) for (k = 0; k < 6; k++) m_histheur[i][j][k] = 0;

	// Reset killers
	for (j = 0; j < PLAYER_MATEDEPTH; j++) {
		m_killers[j].m_scores[0] = -S_INFINITY;
		m_killers[j].m_moves[0] = MOVE_EMPTY;

		m_killers[j].m_scores[1] = -S_INFINITY;
		m_killers[j].m_moves[1] = MOVE_EMPTY;
	}

	// Iterative Deepening
	m_tnodes = m_nodes = m_qnodes = m_tthits = m_ttcutoff = m_failhigh = m_faillow = m_nullcut = m_ttreplace = 0;
	m_ttmove = m_draws = m_egtbhit = m_egtbprobe = 0;
	m_eval->reset_evalstats();
	i = m_options[PLAYER_IDSTART]; 
	alpha = -S_INFINITY; beta = S_INFINITY;
	while (i < PLAYER_MATEDEPTH) {

		m_curiddepth = PLAYER_PLY_FULL(i); m_earlyabort = false;
		m_curroot = 0;

		//AB setup
		tree.m_depth = m_curiddepth;
		tree.m_nullmove = false; tree.m_iid = true;
		tree.m_earlyallowed = (i != m_options[PLAYER_IDSTART]); 
		tree.m_endgame = (board.get_gamestage() == GAME_ENDING);
		tree.m_alpha = alpha; tree.m_beta = beta;
		tree.m_root = true;
		tree.m_capsqr = -1;
		tree.m_extended = false;
		tree.m_descent = 0;
		tree.m_lastmove = MOVE_EMPTY;

		for (j = 0; j < PLAYER_EXT_MAX; j++)
			tree.m_exts[j] = 0;

		//Run PVS search
		score = pvs(board,&tree);
	
		//Aspiration Heuristic
		if (!m_earlyabort && m_options[PLAYER_ASPIRATION]) {
			if ((score <= alpha) || (score >= beta)) {
				alpha = -S_INFINITY; beta = S_INFINITY;
				tree.m_pv.m_length = 0; 
				continue;
			}
			alpha = score-m_options[PLAYER_ASP_WINDOW]; beta = score+m_options[PLAYER_ASP_WINDOW];
		}

		//Timer abort
		if (m_earlyabort) {
			m_curiddepth -= PLAYER_PLY_WIDTH;
			if (m_options[PLAYER_DISPLAY] != DISPLAY_MANAGER_UCI) {
				WRITEPIPE(string("STATUS: EARLY ABORT: USING PV FROM DEPTH: " + itos(i-1) + "\n").c_str());
				WRITEPIPE("\n");
			}
			break;
		}

		// Save off the PV for ordering later
		gather_pv(board, &m_oldline);
		move = m_oldline.m_moves[0]; //Best move is first in PV
		bscore = score;

		i++;
		// Determine if it is time to stop the ID process
		if (timeup()) break;

		if (m_options[PLAYER_SHOWTHINKING]) {
			(DisplayManager::getInstance(m_options[PLAYER_DISPLAY]))->output_pv(
			PLAYER_PLY_LOGICAL(m_curiddepth),bscore,int((getMS()-m_starttime)*100.0),m_tnodes,board,m_oldline,true);
		}
	}

	stats.m_etthits = m_eval->get_evalstats().m_evaltt;
	stats.m_elapsed = getMS()-m_starttime; stats.m_evals = m_eval->get_evalstats().m_evals;
	stats.m_nodes = m_nodes; stats.m_qnodes = m_qnodes; stats.m_tthits = m_tthits;
	stats.m_pthits = m_eval->get_evalstats().m_pawntt; stats.m_ttcutoff = m_ttcutoff;
	stats.m_failhigh = m_failhigh; stats.m_faillow = m_faillow; stats.m_nullcut = m_nullcut;
	stats.m_ttreplace = m_ttreplace; stats.m_ttmove = m_ttmove; stats.m_draws = m_draws;
	stats.m_egtbhit = m_egtbhit; stats.m_egtbprobe = m_egtbprobe;

	k=0;
	for (j = 0; j < m_options[PLAYER_TRANSSIZE]; j++)
		if (m_transtable[j].m_sig != BB_ZERO)
			k++;

	stats.m_ttdensity = double(k)/double(m_options[PLAYER_TRANSSIZE]);

	if (m_options[PLAYER_SHOWTHINKING]) {
		
		(DisplayManager::getInstance(m_options[PLAYER_DISPLAY]))->output_pv(
		PLAYER_PLY_LOGICAL(m_curiddepth),bscore,int((getMS()-m_starttime)*100.0),m_tnodes,board,m_oldline,true);

		(DisplayManager::getInstance(m_options[PLAYER_DISPLAY]))->output_stats(stats);
	}

	m_oldscore = bscore;
	return m_oldscore;
}

//Determine if the thought process should terminate on time concerns
bool Player::timeup() {
	if (m_movenow || (m_options[PLAYER_USETIMING] && m_timeallowed >= 0)) {
		if (m_movenow || ((getMS() - m_starttime)*1000.0 >= m_timeallowed)) {
			m_earlyabort = true;
			return true;
		}
	}
	return false;
}

//Output a variation
void Player::output_thinking(Board& board, Variation* pv, int score, double time) {

	int itime = (int)(time*100.0);

	(DisplayManager::getInstance(m_options[PLAYER_DISPLAY]))->output_pv
		(PLAYER_PLY_LOGICAL(m_curiddepth),score,itime,get_nodes(),board,*pv,false);
}

void Player::output_excitement(Board& board, int depth, move_t move) {
	Variation var;

	var.m_moves[0] = move; var.m_length = 1;
	DisplayManager::getInstance(m_options[PLAYER_DISPLAY])->output_excitement
				(depth,int((getMS()-m_starttime)*100.0),board,var);
}

int Player::maxhh() {
	int best = 0;
	int i,j,k;
	for (i = 0; i < 66; i++)
		for (j = 0; j < 66; j++)
			for (k = 0; k < 6; k++)
				if (m_histheur[i][j][k] > best)
					best = m_histheur[i][j][k];
	return best;
}

InteriorRecognizer* Player::getRecognizer() {
	return m_recog;
}

int Player::get_rootnodes() {
	return m_roots;
}

int Player::get_currentnode() {
	return m_curroot;
}

move_t Player::get_currentmove() {
	return m_currootmove;
}

int Player::get_elapsed() {
	return (int)(100.0*(getMS()-m_starttime));
}

int Player::get_nodes() {
	return m_tnodes;
}

Variation* Player::get_current_pv() {
	return &m_oldline;
}

int Player::get_ply() {
	return PLAYER_PLY_LOGICAL(m_curiddepth);
}

void Player::reset() {
	m_oldline.m_length = 0; m_tnodes = 0; m_nodes = 0; m_qnodes = 0; m_museq = 0;
	m_recog->reset();
}

