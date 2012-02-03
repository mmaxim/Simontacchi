//Mike Maxim
//The Winboard style display manager

#include <origdisplayman.h>

LegacyDisplayManager::LegacyDisplayManager() {

}

void LegacyDisplayManager::output_startsearch() {

}

void LegacyDisplayManager::output_excitement(int,int,Board&,const Variation&) {

}

void LegacyDisplayManager::output_pv(int depth, int score, int elapsed, int nodes, Board& board, const Variation& var, bool real) {
	
	char pvstr[2048];
	string str="",pgn;
	int i,movenum=1;
	bool outnumber;
	Board sboard;

	outnumber = (board.turn() == BB_WHITE);
	if (!outnumber) 
		str += itos(board.get_nummoves()+1) + "... ";
	
	if (outnumber)
		movenum = board.get_nummoves();
	else
		movenum = board.get_nummoves()+1;

	sboard = board;
	for (i = 0; i < var.m_length; i++) {
		if (outnumber) {
			str += itos(++movenum) + ". ";
		}
		pgn = Notation::MoveToSAN(var.m_moves[i],sboard);
		sboard.move(var.m_moves[i]);
		str += pgn + " ";
		outnumber = !outnumber;
	}
	if (var.m_ht) str += "<TT>";

	sprintf(pvstr,"%d %d %d %d\t%s\n",depth,score,elapsed,nodes,str.c_str());
	WRITEPIPE(pvstr);

}

void LegacyDisplayManager::output_stats(const DisplayStats& stats) {
	WRITEPIPE(string("Nodes: " + itos(stats.m_nodes+stats.m_qnodes)
		+ " QNodes: " + itos(stats.m_qnodes)
		+ " NPS: " 
		+ itos(int((stats.m_nodes+stats.m_qnodes)/(stats.m_elapsed))) + "\n").c_str());
	WRITEPIPE(string("TTHits: " + itos(stats.m_tthits) + " H/N: " + dtos(100.0*(double(stats.m_tthits)/double(stats.m_nodes))) 
		+ "% PawnTT: " + itos(stats.m_pthits) + " P/E: " 
		+ dtos(100.0*(double(stats.m_pthits)/double(stats.m_qnodes))) + "%" 
		+ "\n").c_str());
	WRITEPIPE("\n");
}

void LegacyDisplayManager::output_moveconsider(int side, int depth, int elapsed, int movenumber, int movetotal,
											   int nodes, move_t move, Board& board) {


}
