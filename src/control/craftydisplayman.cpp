//Mike Maxim
//Crafty like display manager

#ifdef WIN32
#pragma warning (disable: 4180)
#pragma warning (disable: 4018)
#endif

#include <craftydisplayman.h>

CraftyDisplayManager::CraftyDisplayManager() {
#ifdef WIN32
  COORD size;

	size.X = 80; size.Y = 8000;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),size);
#endif
}

void CraftyDisplayManager::output_startsearch() {
#ifdef WIN32
  CONSOLE_SCREEN_BUFFER_INFO console;
	COORD curpos;
	HANDLE hstdout;
	DWORD written;
	string str;

	cout << endl;
	hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hstdout,&console);
	curpos = console.dwCursorPosition;

	curpos.X = 3;
	SetConsoleCursorPosition(hstdout,curpos);
	str = "(Depth)";

	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);
	
	curpos.X += 9;
	SetConsoleCursorPosition(hstdout,curpos);
	str = "(Time)";
	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);

	curpos.X += 8;
	SetConsoleCursorPosition(hstdout,curpos);
	str = "(Score)";
	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);

	curpos.X += 10;
	SetConsoleCursorPosition(hstdout,curpos);
	str = "(Variation)\n";
	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);
#endif
}

void CraftyDisplayManager::output_excitement(int depth, int elapsed, Board& board, const Variation& var) {
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO console;
	COORD curpos;
	HANDLE hstdout;
	DWORD written;
	Board sboard;
	string str,pgn="",pvstr="";
	char buffer[256];
	int i,movenum=1;
	bool outnumber;

	outnumber = (board.turn() == BB_WHITE);
	if (!outnumber) 
		pvstr += itos(board.get_nummoves()+1) + "... ";
	
	if (outnumber)
		movenum = board.get_nummoves();
	else
		movenum = board.get_nummoves()+1;

	sboard = board;
	for (i = 0; i < var.m_length; i++) {
		if (outnumber) {
			pvstr += itos(++movenum) + ". ";
		}
		pgn = Notation::MoveToSAN(var.m_moves[i],sboard);
		sboard.move(var.m_moves[i]);
		pvstr += pgn + " ";
		outnumber = !outnumber;
	}
	pvstr = pvstr.substr(0,pvstr.length()-1);

	hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hstdout,&console);
	curpos = console.dwCursorPosition;

	curpos.X = 0;
	SetConsoleCursorPosition(hstdout,curpos);
	clear_line();
	curpos.X = 5;
	SetConsoleCursorPosition(hstdout,curpos);
	if (depth < 10)
		str = " ";
	else
		str ="";
	str += itos(depth);
	str += "->";

	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);
	
	curpos.X += 7;
	SetConsoleCursorPosition(hstdout,curpos);
	sprintf(buffer,"%0.2f",(float)((double)elapsed)/100.0);
	str = string(buffer);
	if (str.length() < 5)
		str = " " + str;
	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);

	curpos.X += 8;
	SetConsoleCursorPosition(hstdout,curpos);
	str = "++";
	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);

	curpos.X += 10;
	SetConsoleCursorPosition(hstdout,curpos);
	str = pvstr + "!!\n";
	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);
#endif
}

void CraftyDisplayManager::output_pv(int depth, int score, int elapsed, int nodes, Board& board, const Variation& var, bool real) {
#ifdef WIN32	
	CONSOLE_SCREEN_BUFFER_INFO console;
	COORD curpos;
	HANDLE hstdout;
	DWORD written;
	vector<string> pvstrs;
	string pvstr="";
	string str="",pgn;
	int i,movenum=1;
	bool outnumber;
	char buffer[256];
	Board sboard;

	outnumber = (board.turn() == BB_WHITE);
	if (!outnumber) 
		pvstr += itos(board.get_nummoves()+1) + "... ";
	
	if (outnumber)
		movenum = board.get_nummoves();
	else
		movenum = board.get_nummoves()+1;

	sboard = board;
	for (i = 0; i < var.m_length; i++) {
		if (outnumber) {
			str = itos(++movenum) + ". ";
			if (pvstr.length() + str.length() > 50) {
				pvstrs.push_back(pvstr);
				pvstr = "";
			}
			pvstr += str;
		}
		pgn = Notation::MoveToSAN(var.m_moves[i],sboard);
		sboard.move(var.m_moves[i]);
		str = pgn + " ";
		if (pvstr.length() + str.length() > 50) {
			pvstrs.push_back(pvstr);
			pvstr = "";
		}
		pvstr += str;
		outnumber = !outnumber;
	}
	if (var.m_ht) {
		str = "<TT>";
		if (pvstr.length() + str.length() > 50) {
			pvstrs.push_back(pvstr);
			pvstr = "";
		}
		pvstr += str;
	}
	if (pvstr.length() > 0)
		pvstrs.push_back(pvstr);

	hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hstdout,&console);
	curpos = console.dwCursorPosition;

	curpos.X = 0;
	SetConsoleCursorPosition(hstdout,curpos);
	clear_line();
	curpos.X = 5;
	SetConsoleCursorPosition(hstdout,curpos);
	if (depth < 10)
		str = " ";
	else
		str ="";
	str += itos(depth);
	if (real)
		str += "->";

	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);
	
	curpos.X += 7;
	SetConsoleCursorPosition(hstdout,curpos);
	sprintf(buffer,"%0.2f",(float)((double)elapsed)/100.0);
	str = string(buffer);
	if (str.length() < 5)
		str = " " + str;
	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);

	curpos.X += 8;
	SetConsoleCursorPosition(hstdout,curpos);
	if (score < PLAYER_MATE_SCORE - PLAYER_MATEDEPTH - 10) {
		sprintf(buffer,"%0.2f",(float)(double(score)/100.0));
		str = "("+string(buffer)+")";
	}
	else
		str = "(mate" + itos((PLAYER_MATE_SCORE-score)) +")";

	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);

	curpos.X += 10;
	SetConsoleCursorPosition(hstdout,curpos);
	for (i = 0; i < pvstrs.size(); i++) {
		str = pvstrs[i];
		WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);
		curpos.Y++;
		SetConsoleCursorPosition(hstdout,curpos);
	}
#endif	
}

void CraftyDisplayManager::output_stats(const DisplayStats& stats) {
#ifdef WIN32
  CONSOLE_SCREEN_BUFFER_INFO console;
	HANDLE hstdout;
	COORD curpos;
	string str;

	hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hstdout,&console);
	curpos = console.dwCursorPosition;

	curpos.X = 0; curpos.Y++;
	SetConsoleCursorPosition(hstdout,curpos);
	
	str = "TNodes: " + itos(stats.m_nodes+stats.m_qnodes) + " Nodes: " + itos(stats.m_nodes) + " QNodes: " + itos(stats.m_qnodes)
		+ " PEvals: " + itos(stats.m_evals) + " NPS: " + itos(int((stats.m_nodes+stats.m_qnodes)/(stats.m_elapsed))) + "\n";
	WRITEPIPE(str.c_str());

	str = "TTHits: " + itos(stats.m_tthits) + " (" + dtos(100.0*(double(stats.m_tthits)/double(stats.m_nodes))).substr(0,4) 
		+ "%) TTCut: " + itos(stats.m_ttcutoff) + " (" + dtos(100.0*(double(stats.m_ttcutoff)/double(stats.m_nodes))).substr(0,4) 
		+ "%) TTRep: " + itos(stats.m_ttreplace) + " TTDens: " + dtos(stats.m_ttdensity*100.0).substr(0,4) + "%" +  
		"\n";
	WRITEPIPE(str.c_str());

	str = "TTMov: " + itos(stats.m_ttmove) + " (" + dtos(100.0*(double(stats.m_ttmove)/double(stats.m_nodes))).substr(0,4) +
		"%) EvHits: " + itos(stats.m_etthits) + " (" + dtos(100.0*(double(stats.m_etthits)/double(stats.m_qnodes))).substr(0,5) 
		+ "%) PwHits: " + itos(stats.m_pthits) + " (" 
		+ dtos(100.0*(double(stats.m_pthits)/double(stats.m_evals))).substr(0,5)  + ")%" 
		+"\n";
	WRITEPIPE(str.c_str());

	str = "NullCut: " + itos(stats.m_nullcut) + " (" + dtos(100.0*(double(stats.m_nullcut)/double(stats.m_nodes))).substr(0,5)  +
		"%) FailHigh: " + itos(stats.m_failhigh) + " (" + dtos(100.0*(double(stats.m_failhigh)/double(stats.m_nodes))).substr(0,5)  +
		"%) FailLow: " + itos(stats.m_faillow) + " (" + dtos(100.0*(double(stats.m_faillow)/double(stats.m_nodes))).substr(0,5)  + "%)\n";
	WRITEPIPE(str.c_str());

	str = "Draws: " + itos(stats.m_draws) + " (" + dtos(100.0*(double(stats.m_draws)/double(stats.m_nodes))).substr(0,5) + 
		"%) TBHit: " + itos(stats.m_egtbhit) + " (" + dtos(100.0*(double(stats.m_egtbhit)/double(stats.m_nodes))).substr(0,5) +
		"%) TBProbe: " + itos(stats.m_egtbprobe) + " (" + dtos(100.0*(double(stats.m_egtbprobe)/double(stats.m_nodes))).substr(0,5) +
		"%)\n\n";
	WRITEPIPE(str.c_str());

#endif
}

void CraftyDisplayManager::output_moveconsider(int side, int depth, int elapsed, int movenumber, int movetotal,
											   int nodes, move_t move, Board& board) {
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO console;
	COORD curpos;
	HANDLE hstdout;
	DWORD written;
	string str;
	char buffer[256];

	hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hstdout,&console);
	curpos = console.dwCursorPosition;

	curpos.X = 0;
	SetConsoleCursorPosition(hstdout,curpos);
	clear_line();
	curpos.X = 5;
	SetConsoleCursorPosition(hstdout,curpos);
	if (depth < 10)
		str = " ";
	else
		str ="";
	str += itos(depth);
	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);
	
	curpos.X += 7;
	SetConsoleCursorPosition(hstdout,curpos);
	sprintf(buffer,"%0.2f",(float)((double)elapsed)/100.0);
	str = string(buffer);
	if (str.length() < 5)
		str = " " + str;
	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);

	curpos.X += 8;
	str ="";
	SetConsoleCursorPosition(hstdout,curpos);
	if (movenumber < 10)
		str = " ";
	
	if (movetotal >= 0)
		str += itos(movenumber) + "/" + itos(movetotal);
	else
		str += itos(movenumber) + "/?";

	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);

	curpos.X += 10;
	SetConsoleCursorPosition(hstdout,curpos);
	str = Notation::MoveToSAN(move,board);
	WriteConsole(hstdout,str.c_str(),(DWORD)str.length(),&written,NULL);
#endif
}

void CraftyDisplayManager::clear_line() {
#ifdef WIN32
	char buffer[80];
	DWORD written;
	int i;
	
	for (i = 0; i < 80; i++) buffer[i] = ' ';
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),buffer,60,&written,NULL);
#endif
}

