//Mike Maxim
//Quiesence search code

#include <player.h>

int Player::quies(Board& board, int alpha, int beta, bool timeout) {

	Board sboard;
	MoveList moves,pmoves;
    MoveList culled;
	move_t curmove;
	int score,delta;
	int it;

	m_qnodes++; m_tnodes++;

	if (timeout && timeup())
		return alpha;

	score = m_eval->eval(board,alpha,beta); // Run the material evaluator to see if we can get an early beta cutoff
	if (score >= beta)
		return beta;
	if (score > alpha)
		alpha = score;

	board.generate_capture_moves(pmoves); // Gen the captures only
	quies_order(board,moves,culled,pmoves); //Order the captures
	
	sboard = board;
	for (it = 0; it < moves.size(); it++) {
		curmove = moves.get(it);

		//Quiescent Futility Pruning
		//If the capture isn't good enough to bring us "near" alpha, don't search the move
		delta = alpha - EVAL_MAX_POSITIONAL_CENT - m_eval->material(board);
		if (board.mat_gain(curmove) < delta)
			continue;

		board.move(curmove); m_museq++;
		if (board.in_check()) { board = sboard; continue; } // Check filter

		score = -quies(board,-beta,-alpha,timeout); // Quiescent recursion
		board = sboard;

	//	board.verify_rotator(curmove);

		if (score >= beta) // Please cutoff
			return beta;
		if (score > alpha) 
			alpha = score;
	}

	return alpha;
}
