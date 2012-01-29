//Mike Maxim
//UCI display manager implementation

#include <ucidisplayman.h>
#include <player.h>

UCIDisplayManager::UCIDisplayManager() {

}

void UCIDisplayManager::output_startsearch() {

}

void UCIDisplayManager::output_excitement(int,int,Board&,const Variation&) {

}

void UCIDisplayManager::output_nodeinfo(int depth, int nodes, int elapsed) {

	string str;
	int nps;

	nps = (int)(double(nodes)/(double(elapsed)/100.0));

	str = "info nodes " + itos(nodes) + " nps " + itos(nps) + " depth " + itos(depth) + " seldepth 40\n";
	WRITEPIPE(str.c_str());

	str = "info nodes " + itos(nodes) + " time " + itos(elapsed*10) + "\n";
	WRITEPIPE(str.c_str());
}

void UCIDisplayManager::output_pv(int depth, int score, int elapsed, int nodes, Board& board, const Variation& var, bool real) {
	
	char pvstr[256];
	string str="",pgn,sstr="";
	int i,nps;
	Board sboard;

	if (score < PLAYER_MATE_SCORE - PLAYER_MATEDEPTH - 10) {
		sstr = "cp " + itos(score);
	}
	else
		sstr = "mate " + itos((PLAYER_MATE_SCORE-score));

	sboard = board;
	for (i = 0; i < var.m_length; i++) {
		pgn = Notation::MoveToXB(var.m_moves[i],sboard);
		sboard.move(var.m_moves[i]);
		str += pgn + " ";
	}

	nps = (int)(double(nodes)/(double(elapsed)/100.0));

	WRITEPIPE(string("info nps " + itos(nps) + "\n").c_str());

	sprintf(pvstr,"info depth %d score %s time %d nodes %d pv %s\n",depth,sstr.c_str(),elapsed*10,nodes,str.c_str());
	WRITEPIPE(pvstr);

}

void UCIDisplayManager::output_stats(const DisplayStats& stats) {
	
}

void UCIDisplayManager::output_moveconsider(int side, int depth, int elapsed, int movenumber, int movetotal,
											int nodes, move_t move, Board& board) {

	string str="";
	int nps;

	if ((double(elapsed)/100.0) < 1.0)
		return;

	nps = (int)(double(nodes)/(double(elapsed)/100.0));

	str = "info currmove "+ Notation::MoveToXB(move,board) + " currmovenumber " + itos(movenumber) + 
		  " depth " + itos(depth) + " seldepth 40" + "\n\n";
	WRITEPIPE(str.c_str());

	str = "info nodes " + itos(nodes) + "\n";
	WRITEPIPE(str.c_str());

	str = "info nps " + itos(nps) + "\n";
	WRITEPIPE(str.c_str());
}
