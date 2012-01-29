//Mike Maxim
//Winboard controller implementation

#ifdef WIN32
#pragma warning (disable: 4180)
#pragma warning (disable: 4018)
#pragma warning (disable: 4311)
#endif

#include <winboardcontroller.h>

WinboardController::WinboardController() {

}

void WinboardController::prompt() {
	if (!m_flags[WBC_FLAGS_GUIMODE]) {
		if (m_board.turn() == BB_WHITE)
			WRITEPIPE(" White");
		else
			WRITEPIPE(" Black");

		if (m_mode == WBC_MODE_MATCH)
			WRITEPIPE("<match>: ");
		else if (m_mode == WBC_MODE_FORCE)
			WRITEPIPE("<force>: ");
		else
			WRITEPIPE("<analyze>: ");
	}
}

void WinboardController::init(ConfigFile* cfg) {

	//Set up initial mode and clock arrangement
	m_mode = WBC_MODE_MATCH;
	m_meclock = &m_clocks[BB_BLACK]; m_opclock = &m_clocks[BB_WHITE];

	//Grab a Win32 thread service
#ifdef WIN32
	m_thrsvc = ThreadService::getInstance(THREAD_SERVICE_WIN32);
#else
    m_thrsvc = ThreadService::getInstance(THREAD_SERVICE_PTH);
#endif

	//Get the log file
	m_logfile = cfg->getValue("LOGFILE");

	//Init the clocks with config file default time
	m_meclock->parseClockString(cfg->getValue("TIME"));
	m_opclock->parseClockString(cfg->getValue("TIME"));

	//Book enabled
	m_flags[WBC_FLAGS_BOOKENABLED] = ("true" == cfg->getValue("USEBOOK"));

	//Set up pondering
	m_flags[WBC_FLAGS_PONDERENABLED] = ("true" == cfg->getValue("PONDER"));
	m_flags[WBC_FLAGS_MOVING] = m_flags[WBC_FLAGS_PONDERING] = false;
	m_flags[WBC_FLAGS_GUIMODE] = false;

	m_board.newGame(); //Setup the board
	m_eval.verbose(); //Turn on talkative mode for our base evaluator
	
	WBC_INSTALL_PROC("new",proc_new);
	WBC_INSTALL_PROC("quit",proc_quit);
	WBC_INSTALL_PROC("xboard",proc_xboard);
	WBC_INSTALL_PROC("hard",proc_hard);
	WBC_INSTALL_PROC("easy",proc_easy);
	WBC_INSTALL_PROC("go",proc_go);
	WBC_INSTALL_PROC("result",proc_result);
	WBC_INSTALL_PROC("time",proc_time);
	WBC_INSTALL_PROC("otim",proc_otime);
	WBC_INSTALL_PROC("computer",proc_computer);
	WBC_INSTALL_PROC("name",proc_name);
	WBC_INSTALL_PROC("ics",proc_ics);
	WBC_INSTALL_PROC("protover",proc_protover);
	WBC_INSTALL_PROC("force",proc_force);
	WBC_INSTALL_PROC("level",proc_level);
	WBC_INSTALL_PROC("hint",proc_hint);
	WBC_INSTALL_PROC("bk",proc_bk);
	WBC_INSTALL_PROC("rating",proc_rating);
	WBC_INSTALL_PROC("post",proc_post);
	WBC_INSTALL_PROC("nopost",proc_nopost);
	WBC_INSTALL_PROC("random",proc_random);
	WBC_INSTALL_PROC("accepted",proc_accepted);
	WBC_INSTALL_PROC("load",proc_load);
	WBC_INSTALL_PROC("playloaded",proc_playloaded);
	WBC_INSTALL_PROC("clocks",proc_clocks);
	WBC_INSTALL_PROC("tofen",proc_tofen);
	WBC_INSTALL_PROC("setboard",proc_setboard);
	WBC_INSTALL_PROC("eval",proc_eval);
	WBC_INSTALL_PROC("genmoves",proc_genmoves);
	WBC_INSTALL_PROC("analyze",proc_analyze);
	WBC_INSTALL_PROC(".",proc_dot);
	WBC_INSTALL_PROC("exit",proc_exit);
	WBC_INSTALL_PROC("bkon",proc_bkon);
	WBC_INSTALL_PROC("bkoff",proc_bkoff);
	WBC_INSTALL_PROC("sd",proc_sd);
	WBC_INSTALL_PROC("show",proc_show);
	WBC_INSTALL_PROC("gencapmoves",proc_gencapmoves);
	WBC_INSTALL_PROC("undo",proc_undo);
	WBC_INSTALL_PROC("remove",proc_remove);
	WBC_INSTALL_PROC("playother",proc_playother);
	WBC_INSTALL_PROC("?",proc_movenow);
	WBC_INSTALL_PROC("config",proc_config);
	WBC_INSTALL_PROC("___move",proc_move);
	WBC_INSTALL_PROC("ttlookup",proc_ttlookup);
	WBC_INSTALL_PROC("see",proc_see);
	WBC_INSTALL_PROC("movegenbench",proc_movegenbench);
	WBC_INSTALL_PROC("gennoncap", proc_gennoncap);
	WBC_INSTALL_PROC("cleartrans", proc_cleartrans);

	WRITEPIPE(string(m_board.toString() + "\n").c_str());
	prompt();
}

void WinboardController::process_command(const string& command) {

	StringTokenizer tok(command," ");
	string cmd;
	vector<string> args,margs;
	int i;

	if (tok.size() == 0) {
		prompt();
		return;
	}
	
	cmd = tok[0];
	for (i = 1; i < tok.size(); i++)
		args.push_back(tok[i]);

	//If we don't have this command, attempt to call it a move
	if (m_proc_map.find(cmd) == m_proc_map.end()) {
		if (cmd.length() < 2)
			throw Exception("Error (Unknown Command): " + cmd);
		margs.push_back(cmd);
		(*this.*m_proc_map["___move"])(margs);	
		if (m_mode == WBC_MODE_FORCE)
			prompt();
	}
	else {
		(*this.*m_proc_map[cmd])(args);
		prompt();
	}
}

bool WinboardController::proc_new(const vector<string>& args) {
	WRITEPIPE("STATUS: Resetting Engine\n");

	//New nukes everything, so let us kill any threads in action and reset flags
	if (m_flags[WBC_FLAGS_MOVING]) {
		m_thrsvc->cancel(m_thinkthread);
		m_flags[WBC_FLAGS_MOVING] = false; 
	}
	
	kill_pondering(0,true); //Take pondering down

	//Reset the current game
	m_curgame.m_name="???"; m_curgame.m_myrating=0;  m_curgame.m_oprating = 0;
	m_curgame.m_moves.clear(); m_curgame.m_computer = false; m_curgame.m_side = BB_BLACK;
	m_curgame.m_event = "LOCAL"; m_curgame.m_saved = false;

	//Reset clocks
	m_meclock->reset(); m_opclock->reset();

	//Reset everything
	while (!m_undostack.empty()) m_undostack.pop();
	m_player.reset();
	m_mode = WBC_MODE_MATCH;

	m_board.newGame(); 

	WRITEPIPE("STATUS: Engine reset and ready for new game\n\n");

	return true;
}

bool WinboardController::proc_go(const vector<string>& args) {

	m_mode = WBC_MODE_MATCH;
	if (!m_flags[WBC_FLAGS_MOVING]) {
		kill_pondering(0,true);
		m_curgame.m_side = m_board.turn();
		m_meclock = &m_clocks[m_board.turn()];
		m_opclock = &m_clocks[m_board.otherTurn()];
		m_meclock->startClock();
		m_flags[WBC_FLAGS_MOVING] = true;
		start_move_thread();
	}
	else {
		WRITEPIPE("Error: go during think. Use force.\n");
		return false;
	}

	return true;
}

bool WinboardController::proc_movenow(const vector<string>& args) {
	if (m_flags[WBC_FLAGS_MOVING])
		m_player.move_now();
	return true;
}

bool WinboardController::proc_cleartrans(const vector<string>& args) {

	m_player.clear_trans();
	WRITEPIPE("STATUS: Tables reset: OK\n");

	return true;
}

bool WinboardController::proc_result(const vector<string>& args) {
	
	string result;
	int i;

	if (args.size() == 0)
		return false;

	for (i = 0; i < args.size(); i++)
		result += args[i] + " ";

	kill_pondering(0,true);
	if (m_flags[WBC_FLAGS_MOVING]) {
		m_thrsvc->cancel(m_thinkthread);
		m_flags[WBC_FLAGS_MOVING] = false;
	}

	m_curgame.m_result = result;
	log_game(m_curgame);

	return true;
}

void* wbc_move_thread(void* params);

void WinboardController::start_move_thread() {

	size_t** args = (size_t**) malloc(sizeof(size_t)*3);

	args[0] = (size_t*) this; args[1] = (size_t*) &m_board; args[2] = (size_t*) 0;
	m_thinkthread = m_thrsvc->create((void*)wbc_move_thread,args);
}

void WinboardController::start_ponder_thread(move_t pondermove) {

	size_t** args = (size_t**) malloc(sizeof(size_t)*3);
    
	args[0] = (size_t*) this; args[1] = (size_t*) &m_pboard; args[2] = (size_t*) 1;

	m_pboard = m_board;
	if (m_player.m_options[PLAYER_SHOWTHINKING])
		WRITEPIPE(string("STATUS: PONDERING: " + Notation::MoveToSAN(pondermove,m_pboard) + "\n").c_str());
	m_pboard.move(pondermove);
	if (m_pboard.winner() != BB_DRAW)
		return;

	m_pondermove = pondermove;
	m_meclock->disable(); m_player.toggleOption(PLAYER_BOOKUSAGE,false);
	m_flags[WBC_FLAGS_PONDERING] = true;

	m_ponderthread = m_thrsvc->create((void*)wbc_move_thread,args);
}

void* wbc_move_thread(void* params) {
	
	move_t move;
	string movestr="";
	int score;
	bool pondert=false,gameover=false;
	double timeallowed;
	Board tboard,*m_board;
	WinboardController* m_controller;

	m_controller = (WinboardController*)((size_t**)params)[0]; 
	m_board = (Board*)((size_t**)params)[1];
	pondert = (1 == (int)((size_t*)params)[2]);
	free(params);

	tboard = *m_board;

	if (m_controller->m_meclock->isEnabled())
		timeallowed = m_controller->allocate_time(m_controller->m_meclock, m_controller->m_opclock) * 10.0;
	else
		timeallowed = -1;

	score = m_controller->m_player.get_move(tboard,move,timeallowed);
	
	//If we get out get_move() for some reason in a ponder thread, just sit here
	if (pondert && m_controller->m_flags[WBC_FLAGS_PONDERING]) while (1);

	if (MOVE_ISBLANKMOVE(move)) {
		WRITEPIPE("STATUS: MAJOR ERROR: FAILED TO SELECT MOVE!\n");
		return NULL;
	}

	if (pondert && m_controller->m_flags[WBC_FLAGS_PONDERING]) {
		WRITEPIPE("STATUS: MAJOR ERROR: PONDER THREAD ABOUT TO AFFECT BOARD!\n");
		return NULL;
	}
	
	movestr = "move " + Notation::MoveToSAN(move,tboard) + " \n";
	gameover = m_controller->apply_move(move,movestr);

	m_controller->m_meclock->stopClock(m_board->get_nummoves());
	m_controller->m_opclock->startClock();
	m_controller->output_clocks();

	//Resign only in hopeless situation only against another computer
	if (score < -EVAL_PCVAL(BB_QUEEN) && m_controller->m_curgame.m_computer)
		WRITEPIPE("resign\n");
	else {
		//Ponder
		if (m_controller->m_flags[WBC_FLAGS_PONDERENABLED] && m_controller->m_player.get_current_pv()->m_length > 1)
			m_controller->start_ponder_thread(m_controller->m_player.get_current_pv()->m_moves[1]);
	}

	if (!gameover)
		WRITEPIPE(string(movestr + "\n").c_str());

	m_controller->m_flags[WBC_FLAGS_MOVING] = false;
	m_controller->prompt();

	return NULL;
}

void WinboardController::apply_and_think(move_t move) {

	m_opclock->stopClock(m_board.get_nummoves());

	if (apply_move(move,"")) {
		m_flags[WBC_FLAGS_MOVING] = false;
	}
	else {
		WRITEPIPE("STATUS: USERMOVE PROCESSED, BEGINNING MAIN THINKING\n");

		m_flags[WBC_FLAGS_MOVING] = true;
		m_meclock->startClock();
		start_move_thread();
	}
}

bool WinboardController::proc_move(const vector<string>& args) {

	MoveList moves,fmoves;
	move_t move;
	bool success = false;

	moves.clear(); fmoves.clear();
	m_board.generate_moves(fmoves);
	m_board.generate_check_filter(moves,fmoves);
	
	//Handle either type of notation
	move = Notation::XBtoMove(args[0],m_board);
	if (move == MOVE_EMPTY) 
		move = Notation::SANtoMove(args[0],m_board,success);
	else
		success = true;

	if (success && moves.exists(move)) {
		if (m_mode == WBC_MODE_MATCH) {
			if (!kill_pondering(move,false))
				apply_and_think(move);
		}
		else if (m_mode == WBC_MODE_FORCE) {
			log_state(move);
			m_board.move(move);
		}
		else if (m_mode == WBC_MODE_ANALYZE) {
			m_thrsvc->cancel(m_thinkthread);
			apply_and_think(move);
		}
		return true;
	}

	if (success) 
		throw Exception("Illegal move (not legal): " + args[0]);
	
	throw Exception("Error (Unknown Command): " + args[0]);
}

bool WinboardController::proc_protover(const vector<string>& args) {
	WRITEPIPE(string("\nfeature myname=\"" + string(NAMESTR) + " v" + string(CVERSION) +"\"\n\n").c_str());
	WRITEPIPE("feature setboard=1\n");
	WRITEPIPE("feature playother=1\n");
	WRITEPIPE("feature analyze=1\n");
	WRITEPIPE("feature name=1\n");
	WRITEPIPE("feature ics=1\n");
	WRITEPIPE("feature colors=0\n");
	WRITEPIPE("feature san=1\n");
    WRITEPIPE("feature sigint=0\n");
	WRITEPIPE("feature done=1\n");
	return true;
}

bool WinboardController::proc_level(const vector<string>& args) {

	int i;
	string command="level ";

	for (i = 0; i < args.size(); i++)
		command += args[i] + " ";

	m_meclock->parseClockString(command);
	m_opclock->parseClockString(command);
	
	return true;
}

bool WinboardController::proc_hint(const vector<string>& args) {
	move_t move;

	move = m_player.get_hint();
	if (!MOVE_ISBLANKMOVE(move))
		WRITEPIPE(("Hint: " + Notation::MoveToSAN(move,m_board) + "\n").c_str());

	WRITEPIPE(string("\n").c_str());
	
	return true;
}

bool WinboardController::proc_bk(const vector<string>& args) {

	BKBoardRecord brec;
	int i;

	if (m_player.get_book()->get_bookmoves(m_board,brec)) {
		for (i = 0; i < (int)brec.m_moves.size(); i++) {
			WRITEPIPE((" Move: " + Notation::MoveToSAN(brec.m_moves[i].m_move,m_board) 
				+ " Prob: " + itos((int)(100.0*double(brec.m_moves[i].m_freq)/double(brec.m_total))) + "%\n").c_str());
		}
	}
	else
		WRITEPIPE(string(" Position Not in Book\n").c_str());

	WRITEPIPE("\n");
	
	return true;
}

bool WinboardController::proc_gencapmoves(const vector<string>& args) {

	MoveList moves,fmoves;
	int i;

	moves.clear(); fmoves.clear();
	m_board.generate_capture_moves(fmoves);
	m_board.generate_check_filter(moves,fmoves);
	for (i = 0; i < moves.size(); i++)
		cout << Notation::MoveToSAN(moves.get(i),m_board) << endl;

	return true;
}

bool WinboardController::proc_bkoff(const vector<string>& args) {
	m_player.toggleOption(PLAYER_BOOKUSAGE,false);
	m_flags[WBC_FLAGS_BOOKENABLED] = false;

	return true;
}

bool WinboardController::proc_bkon(const vector<string>& args) {
	m_flags[WBC_FLAGS_BOOKENABLED] = true;
	m_player.toggleOption(PLAYER_BOOKUSAGE,true);
	
	return true;
}

bool WinboardController::proc_sd(const vector<string>& args) {
	
	
	return true;
}

bool WinboardController::proc_show(const vector<string>& args) {
	WRITEPIPE(string(m_board.toString() + "\n").c_str());

	return true;
}

bool WinboardController::proc_accepted(const vector<string>& args) {
	return true;
}

bool WinboardController::proc_random(const vector<string>& args) {
	return true;
}

bool WinboardController::proc_ics(const vector<string>& args) {
	
	if (args.size() != 1)
		return false;
	
	if (args[0] == "-")
		m_curgame.m_event = "LOCAL";
	else
		m_curgame.m_event = args[0];

	return true;
}

bool WinboardController::proc_rating(const vector<string>& args) {

	if (args.size() != 2)
		return false;

	m_curgame.m_myrating = atoi(args[0].c_str());
	m_curgame.m_oprating = atoi(args[1].c_str());

	return true;
}

bool WinboardController::proc_tofen(const vector<string>& args) {

	WRITEPIPE(string(m_board.toFEN() + "\n").c_str());

	return true;
}

bool WinboardController::proc_eval(const vector<string>& args) {
	m_eval.eval(m_board,-S_INFINITY,S_INFINITY);

	return true;
}

bool WinboardController::proc_setboard(const vector<string>& args) {

	string fen;
	int i;

	for (i = 0; i < args.size(); i++) {
		fen += args[i];
		if (i != args.size()-1)
			fen += " ";
	}

	if (m_board.fromFEN(fen))
		WRITEPIPE(string(m_board.toString() + "\n").c_str());
	else
		WRITEPIPE(string("Error (Process Error): Invalid FEN string\n").c_str());

	return true;
}

bool WinboardController::proc_genmoves(const vector<string>& args) {
	
	MoveList moves,fmoves;
	int i;
	
	moves.clear(); fmoves.clear();
	m_board.generate_moves(moves);
	m_board.generate_check_filter(fmoves,moves);
	cout << "N: " << fmoves.size() << " {";
	for (i = 0; i < fmoves.size(); i++) {
		cout << Notation::MoveToSAN(fmoves.get(i),m_board);
		if (i < fmoves.size()-1)
			cout << ",";
	}
	cout << "}" << endl;

	return true;
}

bool WinboardController::proc_gennoncap(const vector<string>& args) {
	
	MoveList moves,fmoves;
	int i;
	
	moves.clear(); fmoves.clear();
	m_board.generate_non_capture_moves(moves);
	m_board.generate_check_filter(fmoves,moves);
	cout << "N: " << fmoves.size() << " {";
	for (i = 0; i < fmoves.size(); i++) {
		cout << Notation::MoveToSAN(fmoves.get(i),m_board);
		if (i < fmoves.size()-1)
			cout << ",";
	}
	cout << "}" << endl;

	return true;
}


bool WinboardController::proc_name(const vector<string>& args) {
	if (args.size() != 1)
		return false;

	m_curgame.m_name = args[0];
	
	return true;
}

bool WinboardController::proc_computer(const vector<string>& args) {
	m_curgame.m_computer = true;	
	return true;
}	

bool WinboardController::proc_load(const vector<string>& args) {

	double starttime;

	starttime = getMS();
	m_pgnloader.load(args[0]);

	WRITEPIPE(string("Loaded Games: " + itos(m_pgnloader.num_games()) + "\n").c_str());
	WRITEPIPE(string("Load Time: " + dtos(getMS()-starttime) + "s\n").c_str());

	return true;
}

bool WinboardController::proc_playloaded(const vector<string>& args) {
	m_pgnloader.play_game(atoi(args[0].c_str()));
	return true;
}

bool WinboardController::proc_clocks(const vector<string>& args) {
	output_clocks();
	return true;
}

bool WinboardController::proc_force(const vector<string>& args) {
	m_mode = WBC_MODE_FORCE;
	if (m_flags[WBC_FLAGS_MOVING]) {
		m_thrsvc->cancel(m_thinkthread);
		m_flags[WBC_FLAGS_MOVING] = false;
	}
	kill_pondering(0,true);

	m_meclock->stopClock(m_board.get_nummoves()); m_opclock->stopClock(m_board.get_nummoves());

	return true;
}	

bool WinboardController::proc_analyze(const vector<string>& args) {

	m_mode = WBC_MODE_ANALYZE;
	m_player.toggleOption(PLAYER_BOOKUSAGE,false);
	m_meclock->disable();

	m_flags[WBC_FLAGS_MOVING] = true;
	start_move_thread();

	return true;
}

bool WinboardController::proc_dot(const vector<string>& args) {

	char buffer[256];

	if (m_mode == WBC_MODE_ANALYZE) {
		sprintf(buffer,"stat01: %d %d %d %d %d %s\n",m_player.get_elapsed(),m_player.get_nodes(),
			m_player.get_ply(),m_player.get_rootnodes()-m_player.get_currentnode(),m_player.get_rootnodes(),
			Notation::MoveToSAN(m_player.get_currentmove(),m_board).c_str());
		WRITEPIPE(buffer);
	}
	return true;
}

bool WinboardController::proc_exit(const vector<string>& args) {
	if (m_mode != WBC_MODE_ANALYZE) 
		WRITEPIPE("Error: exit issued outside of analyze mode. Use quit.\n");
	else {
		m_mode = WBC_MODE_MATCH;
		m_flags[WBC_FLAGS_MOVING] = false;
		m_thrsvc->cancel(m_thinkthread);
		m_meclock->enable();
		if (m_flags[WBC_FLAGS_BOOKENABLED])
			m_player.toggleOption(PLAYER_BOOKUSAGE,true);
	}
	return true;
}

bool WinboardController::proc_see(const vector<string>& args) {

	move_t move;
	MoveList moves,pmoves;
	bool succ;
	int res;

	if (args.size() < 1)
		return false;

	m_board.generate_moves(pmoves);
	m_board.generate_check_filter(moves,pmoves);

	move = Notation::SANtoMove(args[0],m_board,succ);
	if (!succ) return false;
	if (!moves.exists(move)) return false;

	res = m_player.see(m_board, MOVE_GETPIECE(move), MOVE_GETFROM(move), MOVE_GETTO(move));

	WRITEPIPE(string("SEE Score: " + itos(res) + "\n").c_str());

	return true;
}

bool WinboardController::proc_undo(const vector<string>& args) {
	if (!m_undostack.empty()) {
		m_board = m_undostack.top();
		WRITEPIPE(string(m_board.toString() + "\n").c_str());
		m_undostack.pop(); //m_player.remove_history_position();
		m_player.getRecognizer()->pop_game_position();
	}
	
	return true;
}

bool WinboardController::proc_remove(const vector<string>& args) {
	if (!m_undostack.empty()) {
		m_undostack.pop();
		if (!m_undostack.empty()) {
			m_board = m_undostack.top(); m_undostack.pop();
			m_player.getRecognizer()->pop_game_position(); 
			m_player.getRecognizer()->pop_game_position(); 
			WRITEPIPE(string(m_board.toString() + "\n").c_str());
		}
	}
	
	return true;
}

bool WinboardController::proc_playother(const vector<string>& args) {
	
	kill_pondering(0,true);

	m_meclock = &m_clocks[m_board.otherTurn()];
	m_opclock = &m_clocks[m_board.turn()];
	m_opclock->startClock();
	m_mode = WBC_MODE_MATCH;
	
	return true;
}

bool WinboardController::proc_post(const vector<string>& args) {
	m_player.toggleOption(PLAYER_SHOWTHINKING,true);

	return true;
}

bool WinboardController::proc_nopost(const vector<string>& args) {
	m_player.toggleOption(PLAYER_SHOWTHINKING,false);

	return true;
}

bool WinboardController::proc_time(const vector<string>& args) {
	m_meclock->setTimeLeft(atoi(args[0].c_str()));
	return true;
}

bool WinboardController::proc_otime(const vector<string>& args) {
	m_opclock->setTimeLeft(atoi(args[0].c_str()));
	return true;
}

bool WinboardController::proc_xboard(const vector<string>& args) {
	m_player.toggleOption(PLAYER_DISPLAY,DISPLAY_MANAGER_LEGACY);
	m_flags[WBC_FLAGS_GUIMODE] = true;
	return true;
}

bool WinboardController::proc_quit(const vector<string>& args) {
	WRITEPIPE("STATUS: Shutting Down...\n");
	exit(0);
}

bool WinboardController::proc_hard(const vector<string>& args) {
	m_flags[WBC_FLAGS_PONDERENABLED] = true;
	return true;	
}

bool WinboardController::proc_easy(const vector<string>& args) {
	m_flags[WBC_FLAGS_PONDERENABLED] = false;
	kill_pondering(0,true);
	return true;	
}

bool WinboardController::proc_config(const vector<string>& args) {

	m_player.show_config();

	return true;
}

bool WinboardController::proc_ttlookup(const vector<string>& args) {

	Board board;
	TTEntry* ent=NULL;
	string fen;
	int i;

	for (i = 0; i < args.size(); i++) {
		fen += args[i];
		if (i != args.size()-1)
			fen += " ";
	}

	board.fromFEN(fen);
	WRITEPIPE(board.toString().c_str()); WRITEPIPE("\n");

	for (i = 0; i < m_player.m_options[PLAYER_TRANSSIZE]; i++) {
		if (m_player.m_transtable[i].m_sig == board.get_signature()) {
			ent = &m_player.m_transtable[i];
		}
	}

	cout << "ZOBRIST: " << board.hash() << endl;
	if (ent == NULL)
		WRITEPIPE("No such entry!\n");
	else {
		WRITEPIPE(string("Slot: " + itos(board.hash() % m_player.m_options[PLAYER_TRANSSIZE]) + "\n").c_str());
		WRITEPIPE(string("Slot Address: " + itos((size_t)&m_player.m_transtable[board.hash() % m_player.m_options[PLAYER_TRANSSIZE]]) + "\n").c_str());
		WRITEPIPE(string("Position: " + args[0] + "\n").c_str()); 
		WRITEPIPE(string("Board Signature: " + dwtos(board.get_signature()) + "\n").c_str());
		WRITEPIPE(string("Table Signature: " + dwtos(ent->m_sig) + "\n").c_str());
		WRITEPIPE(string("Move: " + Notation::MoveToSAN(ent->m_info.m_move,board) + "\n").c_str());
		WRITEPIPE(string("Check: " + btos(ent->m_info.m_check) + "\n").c_str());
	}

	return true;
}

bool WinboardController::proc_movegenbench(const vector<string>& args) {
	test_movegen();
	return true;
}

bool WinboardController::apply_move(move_t move, const string& movestr) {
	
	int winner,val;

	log_state(move); //Log state
	m_board.move(move); //Make the move on the global board

	WRITEPIPE(string(m_board.toString() + "\n").c_str());
	
	//Check for known draw conditions
	val = m_player.getRecognizer()->probe(m_board, winner);
	if (INTRECOG_ISDRAW(val)) {
		if (movestr.length() > 0) 
			WRITEPIPE(string(movestr + "\n").c_str());

		switch (INTRECOG_DRAWTYPE(val)) {
		case INTRECOG_DRAW_REP:
			WRITEPIPE("1/2-1/2 {3-fold repetition}\n");
			m_curgame.m_result = "1/2-1/2";
			return true;
		case INTRECOG_DRAW_MAT:
			WRITEPIPE("1/2-1/2 {Insufficient material}\n");
			m_curgame.m_result = "1/2-1/2";
			return true;
		case INTRECOG_DRAW_50:
			WRITEPIPE("1/2-1/2 {Fifty move rule}\n");
			m_curgame.m_result = "1/2-1/2";
			return true;
		};
	}

	//Check for stalemate
	if (m_board.stalemate()) {
		if (movestr.length() > 0) 
			WRITEPIPE(string(movestr + "\n").c_str());
		WRITEPIPE("1/2-1/2 {Stalemate}\n");
		m_curgame.m_result = "1/2-1/2";
		return true;
	}

	//Check for checkmate
	winner = m_board.winner();
	if (winner == BB_WHITE) {
		if (movestr.length() > 0) 
			WRITEPIPE(string(movestr + "\n").c_str());
		WRITEPIPE("1-0 {White mates}\n");
		m_curgame.m_result = "1-0";
		return true;
	}
	else if (winner == BB_BLACK) {
		if (movestr.length() > 0) 
			WRITEPIPE(string(movestr + "\n").c_str());
		WRITEPIPE("0-1 {Black mates}\n");
		m_curgame.m_result = "0-1";
		return true;
	}

	return false;
}

void WinboardController::log_state(move_t move) {
	//Log the state before the move in all necessary places
	m_undostack.push(m_board);
//	m_player.add_history_position(m_board); 
	m_player.getRecognizer()->push_game_position(m_board, move);
	m_curgame.m_moves.push_back(move);
}

bool WinboardController::kill_pondering(move_t move, bool justkill) {
	double timeleft;
	if (m_flags[WBC_FLAGS_PONDERING]) {
		
		m_flags[WBC_FLAGS_PONDERING] = false; 
		m_player.toggleOption(PLAYER_BOOKUSAGE,m_flags[WBC_FLAGS_BOOKENABLED]);
		
		if (!justkill && MOVE_ISEQUAL(move,m_pondermove)) {
			WRITEPIPE("STATUS: PONDER MOVE SELECTED\n");
			//If the ponder move happenned to end the game
			//Don't keep thinking
			if (apply_move(move,"")) {
				m_meclock->enable();
				m_thrsvc->cancel(m_ponderthread);
				return true;
			}
			m_pondermove = -1;
			m_meclock->startClock();
			m_meclock->enable();
			timeleft = (allocate_time(m_meclock,m_opclock) * 10.0) * 0.5;
			m_player.modify_timeleft(timeleft);

			m_thinkthread = m_ponderthread;
			m_flags[WBC_FLAGS_MOVING] = true;
			return true;
		}
		else {
			m_pondermove = -1;
			m_meclock->enable();
			m_thrsvc->cancel(m_ponderthread);
			return false;
		}
	}
	return false;
}

void WinboardController::log_game(GameLogEntry& ent) {

	Game game;
	if (!ent.m_saved) {
		game.set_moves(ent.m_moves);

		game.setResult(ent.m_result);
		game.setEvent(ent.m_event);
		game.setElo(ent.m_side,ent.m_myrating);
		game.setName(ent.m_side,"Simontacchi");
		game.setElo(1-ent.m_side,ent.m_oprating);
		game.setName(1-ent.m_side,ent.m_name);

		game.commit(m_logfile);
		ent.m_saved = true;
	}
}

void WinboardController::output_clocks() {
	string wstr,bstr;

	if (m_meclock == &m_clocks[BB_WHITE]) {
		wstr = "(engine)";
		bstr = "(opp)";
	}
	else {
		wstr = "(opp)";
		bstr = "(engine)";
	}

	WRITEPIPE(string("STATUS: WHITE CLOCK" + wstr + ": " + m_clocks[BB_WHITE].toString() 
		+ " BLACK CLOCK" + bstr + ": " + m_clocks[BB_BLACK].toString() + "\n").c_str());
}

Player* WinboardController::getPlayer() {
	return &m_player;
}

Board* WinboardController::getBoard() {
	return &m_board;
}
