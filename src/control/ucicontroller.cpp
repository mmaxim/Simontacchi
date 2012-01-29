//Mike Maxim
//UCI controller implementation

#pragma warning (disable: 4180)
#pragma warning (disable: 4018)
#pragma warning (disable: 4311)

#include <ucicontroller.h>
#ifdef WIN32
#include <windows.h>
#endif

UCIController::UCIController() : m_ponderenabled(false), m_thinking(false) {

}

void UCIController::process_command(const string& command) {

	StringTokenizer tok(command," ");
	string cmd;

	if (tok.size() == 0) {
		prompt();
		return;
	}

	//Check for command support
	cmd = tok[0];
	if (m_proc_map.find(cmd) != m_proc_map.end()) {
		(*this.*m_proc_map[cmd])(command);
		prompt();
	}
}

void UCIController::init(ConfigFile* cfg) {
	//Set options later when I get the uci init command
	m_cfg = cfg;

	//Grab a Win32 thread service
#ifdef WIN32
	m_thrsvc = ThreadService::getInstance(THREAD_SERVICE_WIN32);
#endif

	m_thinking = false;
	m_board.newGame();

	//Init processrors
	UCI_INSTALL_PROC("uci",proc_uci);
	UCI_INSTALL_PROC("setoption",proc_setoption);
	UCI_INSTALL_PROC("isready",proc_isready);
	UCI_INSTALL_PROC("quit",proc_quit);
	UCI_INSTALL_PROC("position",proc_position);
	UCI_INSTALL_PROC("go",proc_go);
	UCI_INSTALL_PROC("stop",proc_stop);
	UCI_INSTALL_PROC("ponderhit",proc_ponderhit);
}

void UCIController::prompt() {
	//UCI controller does not have a prompt (GUI does everything!)
}

Board* UCIController::getBoard() {
	return &m_board;
}

Player* UCIController::getPlayer() {
	return &m_player;
}

bool UCIController::proc_uci(const string& cmdstr) {

	m_player.toggleOption(PLAYER_DISPLAY, DISPLAY_MANAGER_UCI);

	//Engine info
	WRITEPIPE(string(string("\nid name Simontacchi ") + CVERSION + string("\n")).c_str());
	WRITEPIPE("id author Mike Maxim\n");

	//Options
	m_ponderenabled = ("true" == m_cfg->getValue("PONDER"));
	WRITEPIPE(string("option name Clear Hash type button\n").c_str());
	WRITEPIPE(string("option name Ponder type check default " + m_cfg->getValue("PONDER") + "\n").c_str());
	WRITEPIPE(string("option name OwnBook type check default true\n").c_str());
	WRITEPIPE(string("option name BookFile type string default " + m_cfg->getValue("BOOK") + "\n").c_str());
	WRITEPIPE(string("option name NullMove type check default " + m_cfg->getValue("NULLENABLED") + "\n").c_str());
	WRITEPIPE(string("option name NullDepth type string default " + m_cfg->getValue("NULLDEPTH") + "\n").c_str());
	WRITEPIPE(string("option name Hash type spin default " + 
		m_cfg->getValue("TRANSSIZE") + " min 1 max 1024\n").c_str());
	WRITEPIPE(string("option name PawnTranspositionTableSize type spin default " + 
		m_cfg->getValue("PTRANSSIZE") + " min 1 max 64\n").c_str());
	WRITEPIPE(string("option name EvaluationCacheSize type spin default " + 
		m_cfg->getValue("EVALCACHESIZE") + " min 1 max 64\n").c_str());
	WRITEPIPE(string("option name IterativeDeepeningStart type spin default " + 
		m_cfg->getValue("IDSTART") + " min 1 max 10\n").c_str());
	WRITEPIPE(string("option name Contempt type string default " + 
		m_cfg->getValue("CONTEMPT") + "\n").c_str());

	WRITEPIPE(string("option name LazyEvaluation type check default " + 
		m_cfg->getValue("LAZYEVAL") + "\n").c_str());
	WRITEPIPE(string("option name LazyEvaluationSelectivity type spin default " + 
		m_cfg->getValue("LAZYWINDOW") + " min 1 max 9\n").c_str());

	WRITEPIPE(string("option name AspirationEnabled type check default " + 
		m_cfg->getValue("ASPIRATION") + "\n").c_str());
	WRITEPIPE(string("option name AspirationWindow type spin default " + 
		m_cfg->getValue("ASPWINDOW") + " min 1 max 900\n").c_str());

	WRITEPIPE(string("option name ExtendedFutilityWindow type spin default " + 
		m_cfg->getValue("EXTFUTILE") + " min 1 max 900\n").c_str());
	WRITEPIPE(string("option name Razor type spin default " + 
		m_cfg->getValue("RAZOR") + " min 1 max 2000\n").c_str());

	WRITEPIPE("debug on\n");

	WRITEPIPE("uciok\n");

	return true;
}

bool UCIController::proc_position(const string& cmdstr) {

	StringTokenizer tokens(cmdstr, " ");
	string postype,fenstr,str="";
	move_t move;
	int i=0;

	postype = tokens[1];

	//Set up the position
	if (postype == "startpos") {
		m_board.newGame(); i = 2;
		if (tokens.size() > 2)
			str = tokens[i++]; //moves
	}
	else if (postype == "fen") {
		i = 2;
		while (i < tokens.size() && "moves" != (str = tokens[i++])) {		
			fenstr += str + " ";
		}
		m_board.fromFEN(fenstr);
	}

	//Gather movelist
	if (str == "moves") {
		for ( ; i < tokens.size(); i++) {
			str = tokens[i]; //Move
			move = Notation::XBtoMove(str, m_board);
			if (move == MOVE_EMPTY) {
				WRITEPIPE("UCICONTROLLER: Illegal Move List\n");
				return false;
			}
			m_player.getRecognizer()->push_game_position(m_board, move);
			m_board.move(move);
		}
	}

	return true;
}

bool UCIController::proc_stop(const string& cmdstr) {
	
	m_meclock.stopClock(9);
	m_meclock.enable();
	
	if (m_thinking)
		m_player.move_now();

	WRITEPIPE("\n");

	return true;
}

bool UCIController::proc_ponderhit(const string& cmdstr) {
	
	double timeelapsed;

	m_meclock.enable();
	timeelapsed = m_meclock.getElapsedTime();
	
	if (m_timeallocated * 2.0 < timeelapsed)
		m_player.move_now();
	else
		m_player.modify_timeleft(m_timeallocated * 0.5);

	return true;
}

bool UCIController::proc_go(const string& cmdstr) {

	StringTokenizer tokens(cmdstr," ");
	int i;
	
	m_meclock.enable(); m_opclock.enable();
	for (i = 1; i < tokens.size(); i++) {
		if (tokens[i] == "wtime") {
			if (m_board.turn() == BB_WHITE)
				m_meclock.setTimeLeft(atoi(tokens[i+1].c_str())/10);
			else if (m_board.turn() == BB_BLACK)
				m_opclock.setTimeLeft(atoi(tokens[i+1].c_str())/10);
		}
		else if (tokens[i] == "btime") {
			if (m_board.turn() == BB_BLACK)
				m_meclock.setTimeLeft(atoi(tokens[i+1].c_str())/10);
			else if (m_board.turn() == BB_WHITE)
				m_opclock.setTimeLeft(atoi(tokens[i+1].c_str())/10);
		}
		else if (tokens[i] == "winc") {
			if (m_board.turn() == BB_WHITE)
				m_meclock.setIncrement(atoi(tokens[i+1].c_str())/10);
			else if (m_board.turn() == BB_BLACK)
				m_opclock.setIncrement(atoi(tokens[i+1].c_str())/10);
		}
		else if (tokens[i] == "binc") {
			if (m_board.turn() == BB_BLACK)
				m_meclock.setIncrement(atoi(tokens[i+1].c_str())/10);
			else if (m_board.turn() == BB_WHITE)
				m_opclock.setIncrement(atoi(tokens[i+1].c_str())/10);
		}
		else if (tokens[i] == "movestogo") {
			m_meclock.setMovesLeft(atoi(tokens[i+1].c_str()));
			m_opclock.setMovesLeft(atoi(tokens[i+1].c_str()));
		}
		else if (tokens[i] == "infinite" || tokens[i] == "ponder") {
			m_meclock.disable();
			m_opclock.disable();
		}
		else if (tokens[i] == "mate" || tokens[i] == "movetime" || tokens[i] == "nodes" ||
				 tokens[i] == "searchmoves")
			WRITEPIPE("UCICONTROLLER: Operation Not Supported\n");
	}

	start_thinking();
	
	return true;
}

void* uci_move_thread(void* params) {
	
	move_t move;
	string movestr="";
	int score;
	bool pondert=false;
	UCIController* m_controller;
	double timeallowed;

	m_controller = (UCIController*)((int**)params)[0]; 
	free(params);

	m_controller->m_timeallocated = m_controller->allocate_time(&m_controller->m_meclock, &m_controller->m_opclock) * 10.0;
	if (m_controller->m_meclock.isEnabled())
		timeallowed = m_controller->m_timeallocated;
	else
		timeallowed = -1;

	score = m_controller->m_player.get_move(m_controller->m_board,move, timeallowed);
	
	//If we get out get_move() for some reason in infinite though, just sit here
	if (!m_controller->m_meclock.isEnabled()) while (1);

	movestr = "bestmove " + Notation::MoveToXB(move,m_controller->m_board);

	//Ponder
	if (m_controller->m_ponderenabled && m_controller->m_player.get_current_pv()->m_length > 1)
		movestr += " ponder " + 
			Notation::MoveToXB(m_controller->m_player.get_current_pv()->m_moves[1], m_controller->m_board) + "\n";
	else
		movestr += "\n";

	//Say the move
	WRITEPIPE(movestr.c_str());

	m_controller->m_thinking = false;
	
	return NULL;
}

void UCIController::start_thinking() {

	int** args = (int**) malloc(sizeof(int)*1);

	m_thinking = true;
	m_meclock.startClock();

	args[0] = (int*) this; 
	m_thinkthread = m_thrsvc->create((void*)uci_move_thread,args);	
}

bool UCIController::proc_setoption(const string& cmdstr) {
	
	StringTokenizer tokens(cmdstr," ");
	string option;

	if (tokens.size() < 5)
		return true;
	option = tokens[2];

	if (option == "Ponder") 
		m_ponderenabled = ("true" == tokens[4]);
	else if (option == "BookFile")
		m_player.change_books(tokens[4]);
	else if (option == "NullMove") 
		m_player.toggleOption(PLAYER_NULLENABLED, ("true" == tokens[4]));
	else if (option == "NullDepth") 
		m_player.toggleOption(PLAYER_NULLDEPTH, PLAYER_PLY_FULL(atoi(tokens[4].c_str())));
	else if (option == "Hash")
		m_player.toggleOption(PLAYER_TRANSSIZE, (atoi(tokens[4].c_str())*1024*1024)/sizeof(TTEntry));
	else if (option == "PawnTranspositionTableSize")
		m_player.toggleOption(PLAYER_PTRANSSIZE,(atoi(tokens[4].c_str())*1024*1024)/sizeof(PawnHashEntry));
	else if (option == "EvaluationCacheSize")
		m_player.toggleOption(PLAYER_EVALCACHESIZE,(atoi(tokens[4].c_str())*1024*1024)/sizeof(EvalCacheEntry));
	else if (option == "IterativeDeepeningStart")
		m_player.toggleOption(PLAYER_IDSTART, atoi(tokens[4].c_str()));
	else if (option == "Contempt")
		m_player.toggleOption(PLAYER_CONTEMPT, atoi(tokens[4].c_str()));
	else if (option == "LazyEvaluation")
		m_player.toggleOption(PLAYER_LAZYEVAL, ("true" == tokens[4]));
	else if (option == "LazyEvaluationSelectivity")
		m_player.toggleOption(PLAYER_LAZYWINDOW, atoi(tokens[4].c_str()));
	else if (option == "AspirationEnabled")
		m_player.toggleOption(PLAYER_ASPIRATION, ("true" == tokens[4]));
	else if (option == "AspirationWindow")
		m_player.toggleOption(PLAYER_ASP_WINDOW, atoi(tokens[4].c_str()));
	else if (option == "ExtendedFutilityWindow")
		m_player.toggleOption(PLAYER_EXTFUTILE, atoi(tokens[4].c_str()));
	else if (option == "Razor")
		m_player.toggleOption(PLAYER_RAZOR, atoi(tokens[4].c_str()));
	else if (option == "Clear Hash")
		m_player.clear_trans();
	else {
	//	WRITEPIPE(string("UCICONTROLLER: Failed to process: " + option + "\n").c_str());
	}
	return true;
}

bool UCIController::proc_isready(const string& cmdstr) {

	while (m_thinking); //Wait for the UCI thread to complete

	WRITEPIPE("readyok\n");
	return true;
}

bool UCIController::proc_quit(const string& cmdstr) {
	WRITEPIPE("STATUS: Shutting Down...\n");
	exit(0);
}
