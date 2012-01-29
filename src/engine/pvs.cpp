//Mike Maxim
//PVS search module

#include <player.h>
#include <ucidisplayman.h>

#define PLAYER_TRANS_PASS		0
#define PLAYER_KILLER_PASS		2
#define PLAYER_CAP_PASS			1
#define PLAYER_NORM_PASS		3

//Space saving macros
#define OUTPUT_MOVECONSIDER()	if (tree->m_root) {\
									int tmoves;\
									m_curroot++; m_currootmove = curmove;\
									if (i != PLAYER_TRANS_PASS) tmoves = move_worklist.size(); else tmoves = -1;\
									if (m_options[PLAYER_SHOWTHINKING])\
										DisplayManager::getInstance(m_options[PLAYER_DISPLAY])->output_moveconsider(\
										sboard.turn(),PLAYER_PLY_LOGICAL(m_curiddepth),((int)((getMS()-m_starttime)*100.0)),m_curroot,tmoves,get_nodes(),curmove,sboard);\
								}\

int Player::pvs(Board& board, GameTree* tree) {

	MoveList move_worklist,move_culled;
	StoredPosInfo spi;
	GameTree nextlevel;
	Board sboard;
	move_t curmove;
	int i,j,misc,score=0,pmindex=0;
	move_t premoves[3];
	bool mate = true,cap;

	//Ask the interior recognizer oracle if we know about this position
	if (!INTRECOG_ISUNKNOWN((misc = m_recog->probe(board, score)))) {
		if (!(tree->m_root && INTRECOG_ISDRAW(misc))) {
			m_tnodes++; m_nodes++; m_draws++;
			return score;
		}
	}

	//If no more depth available, run the quiescent search
	if (tree->m_depth < PLAYER_PLY_WIDTH) {
		return quies(board,tree->m_alpha,tree->m_beta,tree->m_earlyallowed);
	}
	m_tnodes++; m_nodes++;

	//Get all stored information for this position
	if (get_stored_info(board,tree,spi,move_worklist, tree->m_iid)) {
		m_ttcutoff++;
		return spi.m_val;
	}

	m_recog->add_search_position(board);
	nextlevel.m_root = false; sboard = board; 
	nextlevel.m_earlyallowed = tree->m_earlyallowed; nextlevel.m_root = false;
	nextlevel.m_endgame = tree->m_endgame; nextlevel.m_iid = false; 
	nextlevel.m_descent = tree->m_descent + 1;
	for (i = 0; i < PLAYER_EXT_MAX; i++) nextlevel.m_exts[i] = tree->m_exts[i];

	//Null move pruning
	if (tree->m_nullmove && !tree->m_endgame && !spi.m_check && m_options[PLAYER_NULLENABLED]) { 
		board.toggleTurn(); 
		nextlevel.m_depth = tree->m_depth-PLAYER_PLY_WIDTH-m_options[PLAYER_NULLDEPTH];
		nextlevel.m_beta = -(tree->m_beta-1);
		nextlevel.m_alpha = -tree->m_beta; nextlevel.m_nullmove = false; nextlevel.m_capsqr = -1;
        nextlevel.m_lastmove = MOVE_EMPTY;
		score = -pvs(board,&nextlevel);
		board = sboard;
		if (score >= tree->m_beta) {
			m_nullcut++;
			m_recog->remove_search_position(board);
			return tree->m_beta;
		}
	}
	else
		score = 0;

	//Set up next level tree information
	premoves[0] = premoves[1] = premoves[2] = MOVE_EMPTY;
	if (score < -(PLAYER_MATE_SCORE - 20) && tree->m_exts[PLAYER_EXT_THREAT] < 1) {
		nextlevel.m_exts[PLAYER_EXT_THREAT]++;
		nextlevel.m_depth = tree->m_depth;
	}
	else
		nextlevel.m_depth = tree->m_depth - PLAYER_PLY_WIDTH;
	nextlevel.m_nullmove = true; nextlevel.m_iid = tree->m_iid;

	//Check Extension
	if (spi.m_check && tree->m_exts[PLAYER_EXT_CHECK] < 3) {
		nextlevel.m_depth += PLAYER_PLY_WIDTH;
		nextlevel.m_exts[PLAYER_EXT_CHECK]++;
	}
	//7th rank extension
	if (MOVE_GETPIECE(tree->m_lastmove) == BB_PAWNS && 
		(RANK(MOVE_GETTO(tree->m_lastmove)) == 7 ||
		RANK(MOVE_GETTO(tree->m_lastmove)) == 1) && tree->m_exts[PLAYER_EXT_7THRANK] < 2) {
		nextlevel.m_depth += PLAYER_PLY_WIDTH;
		nextlevel.m_exts[PLAYER_EXT_7THRANK]++;
	}

	//Run PVS recursively in stages
	//Start with the transposition moves, killers, winning captures, then all the rest
	for (i = PLAYER_TRANS_PASS; i <= PLAYER_NORM_PASS; i++) {
		misc = move_worklist.size();
		for (j = 0; j < misc; j++) {
			
			curmove = move_worklist.get(j); //Obtain move board

			if (MOVE_ISEQUAL(curmove,premoves[0]) || MOVE_ISEQUAL(curmove,premoves[1]) ||
				MOVE_ISEQUAL(curmove,premoves[2])) {//Test for repeated moves
				continue;
			}

			if (spi.m_check && (MOVE_ISCASTLE(curmove) || MOVE_ISLONGCASTLE(curmove)))
				continue; 
			cap = board.move(curmove);
			if (board.in_check()) { board = sboard; continue; } // Check filter

			if (i == PLAYER_TRANS_PASS || i == PLAYER_KILLER_PASS) //Log a premove
				premoves[pmindex++] = curmove; 

//			board.verify_rotator(curmove);
			OUTPUT_MOVECONSIDER()

			mate = false;
			nextlevel.m_lastmove = curmove;

			//PVS style recursion
			if (spi.m_type == TT_EXACT && m_options[PLAYER_PVSENABLED]) {
				nextlevel.m_alpha=-(tree->m_alpha+1); 
				nextlevel.m_beta=-tree->m_alpha;
				score = -pvs(board,&nextlevel);
				if ((score > tree->m_alpha) && (score < tree->m_beta)) {
					nextlevel.m_alpha = -tree->m_beta; nextlevel.m_beta = -tree->m_alpha;
					score = -pvs(board,&nextlevel);
				}			
			}
			else {
				nextlevel.m_alpha=-tree->m_beta; 
				nextlevel.m_beta=-tree->m_alpha;
				score = -pvs(board,&nextlevel);
			}

			board = sboard; //Unmake
			if (score >= tree->m_beta) { //Beta cutoff
			//	m_failhigh++;
				spi.m_type = TT_LOWER; spi.m_val = tree->m_beta;
				spi.m_move = curmove;
				trans_store(board, spi, tree->m_root);
				update_hh(curmove, PLAYER_PLY_LOGICAL(tree->m_depth));
				if (!cap) update_killers(curmove, score, tree->m_descent);
				m_recog->remove_search_position(board);
				return tree->m_beta;
			}
			if (score > tree->m_alpha) { //PV node indicator
				tree->m_alpha = score;
				spi.m_move = curmove; spi.m_type = TT_EXACT;
				spi.m_val = score;

				//If we are at the root, output status information
				if (tree->m_root) {
					gather_pv(board,&tree->m_pv, curmove);
					output_thinking(board,&tree->m_pv,score,getMS()-m_starttime);
				}
			}
			if (tree->m_earlyallowed && timeup()) {
				m_recog->remove_search_position(board);
				return tree->m_alpha;
			}
		}

		//Get new moves into the worklist depending on what stage the node is in
		move_worklist.clear();
		switch (i+1) {
		case PLAYER_NORM_PASS: {
				board.generate_non_capture_moves(move_worklist);
				move_worklist.join(move_culled);
				move_order(board, move_worklist); 
			}
			break;
		case PLAYER_CAP_PASS: {
				MoveList caps;
				board.generate_capture_moves(caps);
				quies_order(board, move_worklist, move_culled, caps);
			}
			break;
		case PLAYER_KILLER_PASS: {
			move_t killer; 
			if (board.validate_move((killer = m_killers[tree->m_descent].m_moves[0]))) {
				move_worklist.insert(killer);
			}
			if (board.validate_move((killer = m_killers[tree->m_descent].m_moves[1]))) {
				move_worklist.insert(killer);
			}}
			break;
		default:
			break;
		};
	}

	if (mate && spi.m_check)  { //Mate check 
		m_recog->remove_search_position(board);
		return -(PLAYER_MATE_SCORE-(PLAYER_PLY_LOGICAL(m_curiddepth)-PLAYER_PLY_LOGICAL(tree->m_depth)));
	}
	else if (mate) { //Stalemate protection
		m_recog->remove_search_position(board);
		return m_options[PLAYER_CONTEMPT];
	}

	//Fail low or PV node depending on spi.m_type
	if (spi.m_type == TT_EXACT)
		update_hh(spi.m_move, PLAYER_PLY_LOGICAL(tree->m_depth));
	else
		m_faillow++;

	m_recog->remove_search_position(board);
	trans_store(board, spi, tree->m_root);
	return tree->m_alpha;
}

bool Player::get_stored_info(Board& board, GameTree* tree, StoredPosInfo& spi, MoveList& movelist, bool iid) {
	
	int misc;
	GameTree nextlevel;

	//Probe the main transposition table for stored position information
	if (PLAYER_TT_MISS != (misc = trans_lookup(board,tree->m_depth,tree->m_alpha,tree->m_beta,spi))) {
		//We have a value back from the table and we are not at the root
		m_tthits++;
		if (!tree->m_root && misc == PLAYER_TT_HARDHIT) {
			return true;	
		}
		//See if we have a move and other info
		else {
			if (!MOVE_ISBLANKMOVE(spi.m_move)) {
				m_ttmove++;
				movelist.insert(spi.m_move);
			}
			else if (iid && tree->m_depth > PLAYER_PLY_FULL(PLAYER_PREFRONTIERDEPTH)) {
				//Utilize IID techiniques to attempt to get a hash move
				nextlevel = *tree;
				nextlevel.m_depth = tree->m_depth - PLAYER_PLY_FULL(2);
				nextlevel.m_nullmove = false; nextlevel.m_iid = false;
				
				pvs(board, &nextlevel); //Run with smaller depth for a move
				
				return get_stored_info(board, tree, spi, movelist, false);
			}
			spi.m_type = TT_UPPER; spi.m_depth = tree->m_depth;
		}
	}
	else if (iid && tree->m_depth > PLAYER_PLY_FULL(PLAYER_PREFRONTIERDEPTH)) {
		//Utilize IID techiniques to attempt to get a hash move
		nextlevel = *tree;
		nextlevel.m_depth = tree->m_depth - PLAYER_PLY_FULL(2);
		nextlevel.m_nullmove = false; nextlevel.m_iid = false;

		pvs(board, &nextlevel); //Run with smaller depth for a move
		
		return get_stored_info(board, tree, spi, movelist, false);
	}
	else {
		//Since we did not cutoff via tables, set up positional information
		spi.m_type = TT_UPPER; spi.m_depth = tree->m_depth;
		spi.m_check = board.check(); spi.m_nullmove = true; 
		spi.m_move = MOVE_EMPTY;
        spi.m_val = PLAYER_MATE_SCORE;

	}

	return false;
}

void Player::update_killers(move_t pkiller, int score, int ply) {
	if (score >= m_killers[ply].m_scores[0]) { // If the move caused beta cutoff and is higher than previous, it is the killer
		//Move former highest killer into subordinate slot
		m_killers[ply].m_moves[1] = m_killers[ply].m_moves[0];
		m_killers[ply].m_scores[1] = m_killers[ply].m_scores[0];

		//Set new highest killer
		m_killers[ply].m_moves[0] = pkiller;
		m_killers[ply].m_scores[0] = score;
	}
	else if (score >= m_killers[ply].m_scores[1]) {
		//Set new subordinate killer
		m_killers[ply].m_moves[1] = pkiller;
		m_killers[ply].m_scores[1] = score;
	}
}

void Player::update_hh(move_t move, int depth) {
	m_histheur[MOVE_GETFROM(move)][MOVE_GETTO(move)][MOVE_GETPIECE(move)] += (1<<depth); // HH update for being a good move
}
