//Mike Maxim
//Static Exchange Evaluator

#include <player.h>

int Player::see(Board& oboard, int piece, int from, int to) {

	int swap_list[40],atkpcval,direction,turn,i=1;
	BoardPosition bp;
	bitboard atx,bb;
	Board board;

	board = oboard;
	turn = board.turn();
	
	//Get all attacks for both players on the target square
	atx = board.get_all_attacks_to(to);

	board.get_square(to, bp);
	atkpcval = EVAL_PCVAL(bp.m_type);

	turn=1-turn; board.toggleTurn();
	swap_list[0] = atkpcval;
	atkpcval = EVAL_PCVAL(piece);
	atx = CLEAR_BIT(atx, from);
	direction = board.m_directions[to][from];
	if (direction != BB_NORELATION) see_peek(board, 1-turn, atx, from, to, direction);

	while (atx) {
		if (BB_ZERO != (bb = board.m_pbbs[BB_PAWNS][turn] & atx)) {
			from = BB_NEXTBIT(bb);
//			cout << "SEE: Pawn" << endl;
		}
		else if (BB_ZERO != (bb = board.m_pbbs[BB_KNIGHT][turn] & atx)) {
			from = BB_NEXTBIT(bb);
//			cout << "SEE: Knight" << endl;
		}
		else if (BB_ZERO != (bb = board.m_pbbs[BB_BISHOP][turn] & atx)) {
			from = BB_NEXTBIT(bb);
//			cout << "SEE: Bishop" << endl;
		}
		else if (BB_ZERO != (bb = board.m_pbbs[BB_ROOK][turn] & atx)) {
			from = BB_NEXTBIT(bb);
//			cout << "SEE: Rook" << endl;
		}
		else if (BB_ZERO != (bb = board.m_pbbs[BB_QUEEN][turn] & atx)) {
			from = BB_NEXTBIT(bb);
//			cout << "SEE: Queen" << endl;
		}
		else if (BB_ZERO != (bb = board.m_pbbs[BB_KING][turn] & atx)) {
			from = board.m_kingpos[turn];
//			cout << "SEE: King" << endl;
		}
		else
			break;

		swap_list[i] = -swap_list[i-1] + atkpcval;
		board.get_square(from,bp); atkpcval = EVAL_PCVAL(bp.m_type);
		atx = CLEAR_BIT(atx, from);
		direction = board.m_directions[to][from];
		turn = 1-turn; board.toggleTurn();
		if (direction != BB_NORELATION) see_peek(board, 1-turn, atx, from, to, direction);
		i++;
	}

	while (i--) {
		if (swap_list[i] > -swap_list[i-1]) 
			swap_list[i-1]=-swap_list[i];
	}

	return swap_list[0];
}

void Player::see_peek(Board& board, int turn, bitboard& atx, int from, int to, int direction) {

//	cout << "STATUS: Running see_peek() direction: " << direction << endl;

	switch (direction) {
	case BB_DOWN:
		atx |= (board.get_file_attacks_own(from) & board.m_pbbs[BB_ROOKQUEEN][turn] & 
			    board.m_dirbb[from][BB_DOWN]);
		break;
	case BB_UP:
		atx |= (board.get_file_attacks_own(from) & board.m_pbbs[BB_ROOKQUEEN][turn] & 
			    board.m_dirbb[from][BB_UP]);
		break;
	case BB_RIGHT:
		atx |= (board.get_rank_attacks_own(from) & board.m_pbbs[BB_ROOKQUEEN][turn] & 
			    board.m_dirbb[from][BB_RIGHT]);
		break;
	case BB_LEFT:
		atx |= (board.get_rank_attacks_own(from) & board.m_pbbs[BB_ROOKQUEEN][turn] & 
			    board.m_dirbb[from][BB_LEFT]);
		break;
	case BB_DOWNLEFT:
		atx |= (board.get_diagA1_attacks_own(from) & board.m_pbbs[BB_BISHOPQUEEN][turn] & 
			    board.m_dirbb[from][BB_DOWNLEFT]);
		break;
	case BB_DOWNRIGHT:
		atx |= (board.get_diagH8_attacks_own(from) & board.m_pbbs[BB_BISHOPQUEEN][turn] & 
			    board.m_dirbb[from][BB_DOWNRIGHT]);
		break;
	case BB_UPLEFT:
		atx |= (board.get_diagH8_attacks_own(from) & board.m_pbbs[BB_BISHOPQUEEN][turn] & 
			    board.m_dirbb[from][BB_UPLEFT]);
		break;
	case BB_UPRIGHT:
		atx |= (board.get_diagA1_attacks_own(from) & board.m_pbbs[BB_BISHOPQUEEN][turn] & 
			    board.m_dirbb[from][BB_UPRIGHT]);
		break;
	};
}
