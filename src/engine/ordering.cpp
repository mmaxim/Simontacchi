//Mike Maxim
//Move ordering functionality

#include <player.h>

//TODO: Implement heap-based pqueue. Amortize cost of insertion.
void Player::move_order(Board& board, MoveList& moves) {

	int it;
	move_t move;
	int weights[MOVELIST_MAXMOVES];
	bool badcap;

	// Run through the moves and assign weights for sorting
	for (it = 0; it < moves.size(); it++) {
		move = moves.get(it);
		weights[it] = move_weight(board,move,badcap);
	}
	moves.sort(weights);
}

void Player::quies_order(Board& board, MoveList& qlist, MoveList& culled, const MoveList& captures) {

	int i,index=0,to,from,piece,val;
	move_t move;
	int weights[MOVELIST_MAXMOVES];
	BoardPosition bp;

	for (i = 0; i < captures.size(); i++) {
		move = captures.get(i);

		to = MOVE_GETTO(move); from = MOVE_GETFROM(move); piece = MOVE_GETPIECE(move);

		board.get_square(to,bp);

		//En passant
		if (bp.m_color == BB_DRAW && piece == BB_PAWNS)
			bp.m_type = BB_PAWNS;
	
		//If capturing piece is less value than captured, then that score
		//is good enough.
		val = (EVAL_PCVAL(bp.m_type)-EVAL_PCVAL(piece));
		if (val > 0) {
			qlist.insert(move);
			weights[index++] = val;
		}
		else {
			//If the capturing piece is more valuable than captured, we run
			//the SEE for tha actual exchange value
			if (0 <= (val = see(board, piece, from, to))) {
				qlist.insert(move);
				weights[index++] = val;
			}
			else
				culled.insert(move);
		}
	}

	//Sort everything
	qlist.sort(weights);
}

int Player::move_weight(Board& board, move_t move, bool& badcap) {
	int weight=0,cap=0;
	int to,from,piece;

	to = MOVE_GETTO(move); from = MOVE_GETFROM(move); piece = MOVE_GETPIECE(move); 
	// History Heuristic
	weight = m_histheur[from][to][piece];
	if (board.is_occupied(to)) {
		BoardPosition bp;
		board.get_square(to,bp);

		//En passant
		if (bp.m_color == BB_DRAW && piece == BB_PAWNS)
			bp.m_type = BB_PAWNS;	
		
		//If capturing piece is less value than captured, then that score
		//is good enough.
		cap = (EVAL_PCVAL(bp.m_type)-EVAL_PCVAL(piece));
		if (cap > 0) 
			weight += cap;
		else {
			//If the capturing piece is more valuable than captured, we run
			//the SEE for tha actual exchange value
			weight += (cap = see(board, piece, from, to));
			badcap = (cap < 0);
		}
	}
	
	return weight;
}