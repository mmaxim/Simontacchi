//Mike Maxim
//Transposition table implementation

#include <player.h>

TTEntry* Player::m_transtable = NULL;

//Lookup in the main transposition table for as much info as possible
int Player::trans_lookup(Board& board, int depth, int alpha, int beta, StoredPosInfo& entry) {
	
	TTEntry* ent;

	ent = trans_getentry(board,true);
	if (m_options[PLAYER_TRANSENABLED] && ent != NULL) {
		//Copy the hash table entry into out parameter
		entry = ent->m_info;
		if (ent->m_info.m_depth >= depth) {
			if (ent->m_info.m_type == TT_EXACT) // An exact score can be returned
				return PLAYER_TT_HARDHIT;
			// If we have an upper bound less than alpha no point in continuing (instability?)
			else if (ent->m_info.m_type == TT_UPPER && ent->m_info.m_val < alpha) { 
				entry.m_val = alpha;
				return PLAYER_TT_HARDHIT;
			}
			// If we have a lower bound more than beta no point in continuing (instability?)
			else if (ent->m_info.m_type == TT_LOWER && ent->m_info.m_val > beta) {
				entry.m_val = beta;
				return PLAYER_TT_HARDHIT;
			}
		}
		return PLAYER_TT_SOFTHIT; //Signifies that we got a move back
	}

	return PLAYER_TT_MISS;
}

void Player::trans_store(Board& board, const StoredPosInfo& info, bool pvroot) {	
	
	TTEntry* ent;

	//Weirdness guard
	if (info.m_val <= -PLAYER_MATE_SCORE || info.m_val >= PLAYER_MATE_SCORE) {
		return;
	}

	//Stored the information
	ent = trans_getentry(board,false);
	if (m_options[PLAYER_TRANSENABLED] && !(!pvroot && ent->m_pvroot)) { 
		
		if (ent->m_sig != BB_ZERO && ent->m_sig != board.get_signature())
			m_ttreplace++;

		ent->m_info = info;
		ent->m_sig = board.get_signature();
		ent->m_pvroot = pvroot;
	}
	else if (pvroot) {
		cout << "Failed Root Insertion: SDepth: " << (int)ent->m_info.m_depth
			 << " NDepth: " << (int)info.m_depth << endl;
	}
}

TTEntry* Player::trans_getentry(Board& board, bool lookup) {

	unsigned int index,oindex,i;

	index = oindex = board.hash() % m_options[PLAYER_TRANSSIZE];
	
	//No probing if we replace ourselves or the entry is empty
	if ((!lookup && m_transtable[index].m_sig == BB_ZERO) || m_transtable[index].m_sig == board.get_signature())
		return &m_transtable[index];
	else {
		//Try to probe around the entry for a blank entry or same board
		for (i = 0; i < PLAYER_LIN_DISTANCE; i++) {
			index = (index + 1) % m_options[PLAYER_TRANSSIZE];
			if ((!lookup && m_transtable[index].m_sig == BB_ZERO) || m_transtable[index].m_sig == board.get_signature())
				return &m_transtable[index];
		}
	}

	//Failed to find an entry
	if (lookup)
		return NULL;
	else {
		return &m_transtable[oindex];
	}
}

//Collect the PV from the transposition table
void Player::gather_pv(Board& board, Variation* pv, move_t fmove) {
	
	int i;
	TTEntry* ent;
	Board sboard;

	sboard = board;

	//Deal with the first move
	if (MOVE_ISBLANKMOVE(fmove))
		pv->m_length = 0;
	else {
		pv->m_length = 1;
		pv->m_moves[0] = fmove;
		sboard.move(fmove);
	}

	//Keep proceeding through the table until no more positions are found
	for (i = 0; i < PLAYER_MATEDEPTH; i++) {
		if (NULL != (ent = trans_getentry(sboard, true))) {
			if (MOVE_ISBLANKMOVE(ent->m_info.m_move))
				return;

			pv->m_moves[pv->m_length++] = ent->m_info.m_move;
			sboard.move(ent->m_info.m_move);
		}
		else
			return;
	}
}

