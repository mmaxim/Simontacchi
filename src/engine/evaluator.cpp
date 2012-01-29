//Mike Maxim
//Evaluator implementation

#include <evaluator.h>

PawnHashEntry* Evaluator::m_hashpawn = NULL;
EvalCacheEntry* Evaluator::m_evalcache = NULL;
int Evaluator::m_hashsize = 500;
int Evaluator::m_evalcachesize = 500;
int Evaluator::m_tropism[64][64][BB_MAXPIECE-1];
int Evaluator::m_pvals[BB_MAXPIECE];

Evaluator::Evaluator() : m_verbose(false), m_lazy(false), m_lazywindow(0) {
	if (m_hashpawn == NULL) 
		m_hashpawn = new PawnHashEntry[m_hashsize];
	if (m_evalcache == NULL) 
		m_evalcache = new EvalCacheEntry[m_evalcachesize];
	
	reset_evalstats();
}

Evaluator::Evaluator(bool lazy, int lazywindow) : m_verbose(false), m_lazy(lazy), m_lazywindow(lazywindow) {
	if (m_hashpawn == NULL) 
		m_hashpawn = new PawnHashEntry[m_hashsize];
	if (m_evalcache == NULL) 
		m_evalcache = new EvalCacheEntry[m_evalcachesize];
	
	reset_evalstats();
}

void Evaluator::static_initialization() {
	init_tropism();
	
	//Init piece values
	m_pvals[BB_PAWNS] = EVAL_SCORE_PAWN;
	m_pvals[BB_KNIGHT] = EVAL_SCORE_KNIGHT;
	m_pvals[BB_BISHOP] = EVAL_SCORE_BISHOP;
	m_pvals[BB_ROOK] = EVAL_SCORE_ROOK;
	m_pvals[BB_QUEEN] = EVAL_SCORE_QUEEN;
	m_pvals[BB_KING] = EVAL_SCORE_KING;
}

void Evaluator::init_tropism() {
	
	int i,j,k;
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64; j++) {
			for (k = BB_PAWNS; k <= BB_QUEEN; k++) {
				switch (k) {
				case BB_KNIGHT:
					m_tropism[i][j][k] = EVAL_KNIGHT_TROPISM(i,j);
					break;
				case BB_ROOK:
					m_tropism[i][j][k] = EVAL_ROOK_TROPISM(i,j);
					break;
				case BB_QUEEN:
					m_tropism[i][j][k] = EVAL_QUEEN_TROPISM(i,j);
					break;
				default:
					m_tropism[i][j][k] = 0;
				};
			}
		}
	}
}

void Evaluator::set_pawnsize(int size) {
	m_hashsize = size;
	if (m_hashpawn != NULL) {
		delete [] m_hashpawn;
		m_hashpawn = NULL;
	}
	m_hashpawn = new PawnHashEntry[m_hashsize];
}

void Evaluator::set_evalsize(int size) {
	m_evalcachesize = size;
	if (m_evalcache != NULL) {
		delete [] m_evalcache;
		m_evalcache = NULL;
	}
	m_evalcache = new EvalCacheEntry[m_evalcachesize];
}

EvalStats& Evaluator::get_evalstats() {
	return m_evalstats;
}

void Evaluator::reset_evalstats() {
	m_evalstats.m_evals = m_evalstats.m_evaltt = m_evalstats.m_pawntt = 0;
}

// Material only
int Evaluator::fasteval(Board& board) {
	return material(board);
}

int Evaluator::eval(Board& board, int alpha, int beta) {
	
	int posscore=0,matscore=0;
	EvalCacheEntry* ent;
	
	if ((board.get_signature() == ((ent = &m_evalcache[board.hash() % m_evalcachesize])->m_board)) &&
		(board.get_gamestage() == ent->m_stage) && (board.is_castled(0) == ent->m_hascastled[0]) &&
		(board.is_castled(1) == ent->m_hascastled[1])) {
		m_evalstats.m_evaltt++;
		return ent->m_score;
	}
	
	matscore = material(board);
	
	//Lazy Evaluation
	if (m_lazy && (matscore >= beta+(m_lazywindow*EVAL_PCVAL(BB_PAWNS))))
		return matscore;
	
	m_evalstats.m_evals++;
	if (board.get_gamestage() == GAME_OPENING)
		posscore = opening_positional(board);
	else if (board.get_gamestage() == GAME_MIDDLE)
		posscore = middle_positional(board);
	else
		posscore = end_positional(board);
	
	ent->m_board = board.get_signature();
	ent->m_hascastled[0] = board.is_castled(0);
	ent->m_hascastled[1] = board.is_castled(1);
	ent->m_score = matscore+posscore;
	ent->m_stage = board.get_gamestage();
	
	return ent->m_score;
}

//Evaluate in the opening situation
int Evaluator::opening_positional(Board& board) {
	
/*	int ksafety=0,castles=0,queen=0,pawnscore=0,ptscore=0,ptable=0,dummy,ipos;
double posscore;

  castles = castle(board);
  ptable = (3*piecetable(board,dummy,false))/2;
  ksafety = kingsafety(board);
  pawnscore = pawns(board,ptscore);
  queen = queens(board);
  
	ptable += ptscore;
	posscore = MIN(EVAL_MAX_POSITIONAL_NORM,
	(8.0*(double(ptable)/95.0)+
	8.1*(double(castles)/100.0)+
	1.0*(double(ksafety)/100.0)+
	2.9*(double(queen)/100.0)+
	8.0*(double(pawnscore)/100.0))/28.0);
	
	  ipos = (int)(100.0*posscore);
	  
		if (m_verbose) verbosity(ipos,castles,ptable,queen,pawnscore,ksafety,0);
		
	return ipos;*/
	return middle_positional(board);
}

int Evaluator::middle_positional(Board& board) {
	
	int ksafety,castles,pawnscore=0,ptable,ptscore=0,tropism=0,mobility=0,ipos;
	
	castles = castle(board);
	ptable = loopeval(board,tropism,mobility,false);
	ksafety = kingsafety(board);
	pawnscore = pawns(board,ptscore);
	
	ptable += ptscore;

	//Get mobility in centipawns
	//Large advantage
	if (mobility >= 10 || mobility <= -10) {
		if (mobility < 0) mobility = -25; else mobility = 25;
	}
	//Medium advantage
	else if (mobility >= 5 || mobility <= -5) {
		if (mobility < 0) mobility = -20; else mobility = 20;
	}
	//Small advantage
	else if (mobility >= 1 || mobility <= -1) {
		if (mobility < 0) mobility = -10; else mobility = 10;
	}
	else
		mobility = 0;
	
	ipos = MIN(ptable,40)+MIN(castles,30)+MIN(30,ksafety)+
		MIN(pawnscore,40)+MIN(30,tropism)+MIN(mobility,30);
	
	if (m_verbose) verbosity(ipos,castles,ptable,0,pawnscore,ksafety,tropism,mobility);
	
	return ipos;
}

int Evaluator::end_positional(Board& board) {
	
	int ksafety,castles,pawnscore=0,ptable,ptscore=0,tropism=0,mobility=0,ipos;
	
	castles = castle(board);
	ptable = loopeval(board,tropism,mobility,true);
	ksafety = kingsafety(board);
	pawnscore = pawns(board,ptscore);
	
	ptable += ptscore;

	//Get mobility in centipawns
	//Large advantage
	if (mobility >= 10 || mobility <= -10) {
		if (mobility < 0) mobility = -25; else mobility = 25;
	}
	//Medium advantage
	else if (mobility >= 5 || mobility <= -5) {
		if (mobility < 0) mobility = -20; else mobility = 20;
	}
	//Small advantage
	else if (mobility >= 1 || mobility <= -1) {
		if (mobility < 0) mobility = -10; else mobility = 10;
	}
	else
		mobility = 0;
	
	ipos = MIN(ptable,40)+MIN(castles,30)+MIN(30,ksafety)+
		MIN(pawnscore,40)+MIN(30,tropism)+MIN(mobility,30);
	
	if (m_verbose) verbosity(ipos,castles,ptable,0,pawnscore,ksafety,tropism,mobility);
	
	return ipos;
}

// -95 <= score <= 95 (-120 <= score 120 for endgame)
// -35 <= tropism <= 35
int Evaluator::loopeval(Board& board, int& tropism, int& mobility, bool endgame) {
	
	bitboard bb;
	int score=0;
	int pos,turn,oturn,tk,ok;
	
	turn = board.turn(); oturn = board.otherTurn();
	tk = board.get_kingpos(turn);
	ok = board.get_kingpos(oturn);
	
	for (pos = 0; pos < 64; pos++) { *((char*)&m_wb[pos]) = 0; 
	*((char*)&m_bb[pos]) = 0; }
	
	//Evaluate king centrality in the endgame
	if (endgame) 
		score += m_endkingpos[tk] - m_endkingpos[ok];
	
	//Evaluate knight-bishop piece placement
	score += board.get_piecetable(turn) - board.get_piecetable(oturn);
	
	//Evaluate knight tropism
	bb = board.m_pbbs[BB_KNIGHT][turn];
	while (bb) {
		pos = BB_NEXTBIT(bb);
		tropism += m_tropism[pos][ok][BB_KNIGHT];
		mobility += mobknight(board, pos);
		bb = CLEAR_BIT(bb,pos);
	}
	bb = board.m_pbbs[BB_KNIGHT][oturn];
	while (bb) {
		pos = BB_NEXTBIT(bb);
		tropism -= m_tropism[pos][tk][BB_KNIGHT];
		board.toggleTurn();
		mobility -= mobknight(board, pos);
		board.toggleTurn();
		bb = CLEAR_BIT(bb,pos);
	}
	
	//Evaluate knight tropism
	bb = board.m_pbbs[BB_BISHOP][turn];
	while (bb) {
		pos = BB_NEXTBIT(bb);
		mobility += mobbishop(board, pos);
		bb = CLEAR_BIT(bb,pos);
	}
	bb = board.m_pbbs[BB_BISHOP][oturn];
	while (bb) {
		pos = BB_NEXTBIT(bb);
		board.toggleTurn();
		mobility -= mobbishop(board, pos);
		board.toggleTurn();
		bb = CLEAR_BIT(bb,pos);
	}
	
	//Evaluate on turn rook effectiveness
	bb = board.m_pbbs[BB_ROOK][turn];
	while (bb) {
		mobility += mobrook(board,(pos = BB_NEXTBIT(bb)),turn);
		tropism += m_tropism[pos][ok][BB_ROOK];
		bb = CLEAR_BIT(bb,pos);
	}		
	//Evaluate other turn rook effectiveness
	bb = board.m_pbbs[BB_ROOK][oturn];
	while (bb) {
		board.toggleTurn();
		mobility -= mobrook(board,(pos = BB_NEXTBIT(bb)),oturn);
		board.toggleTurn();
		tropism -= m_tropism[pos][tk][BB_ROOK];
		bb = CLEAR_BIT(bb,pos);
	}
	
	//Evaluate queen tropism
	if (board.m_pbbs[BB_QUEEN][turn]) {
		pos = BB_NEXTBIT(board.m_pbbs[BB_QUEEN][turn]);
		tropism += m_tropism[pos][ok][BB_QUEEN];
		mobility += mobqueen(board, pos);
	}
	
	if (board.m_pbbs[BB_QUEEN][oturn]) {
		pos = BB_NEXTBIT(board.m_pbbs[BB_QUEEN][oturn]);
		tropism -= m_tropism[pos][tk][BB_QUEEN];
		board.toggleTurn();	
		mobility -= mobqueen(board, pos);
		board.toggleTurn();	
	}
	
	return score;
}

int Evaluator::material(Board& board) {
	return board.get_material(board.turn())-board.get_material(board.otherTurn());
}

// -EVAL_PCVAL(BB_PAWNS) <= score <= EVAL_PCVAL(BB_PAWNS)
int Evaluator::pawns(Board& board, int& ptscore) {
	
	int i,j;
	int dpawns[2],posing=0;
	int ipawns[2],ppawns[2],rams[2],turn,oturn;
	bitboard bb,pwns;
	int score;
	PawnHashEntry* phe;
	
	ptscore = 0;
	turn = board.turn(); oturn = board.otherTurn();
	phe = &m_hashpawn[board.pawn_hash() % m_hashsize];
	if ((phe->m_pawns[0] == board.m_pbbs[BB_PAWNS][0]) && 
		(phe->m_pawns[1] == board.m_pbbs[BB_PAWNS][1])) {
		if (turn == phe->m_turn) {
			m_evalstats.m_pawntt++;
			ptscore = phe->m_ptscore;
			return phe->m_score;
		}
		else if (oturn == phe->m_turn) {
			m_evalstats.m_pawntt++;
			ptscore = -phe->m_ptscore;
			return -phe->m_score;
		}
	}
	
	dpawns[0] = dpawns[1] = 0;
	ipawns[0] = ipawns[1] = 0;
	ppawns[0] = ppawns[1] = 0;
	rams[0] = rams[1] = 0;
	
	//Doubled Pawns
	for (i = 0; i < 8; i++) {
		for (j = turn; true; j = oturn) {
			bb = board.get_pawn_column(j,i);
			if (board.bb_count(bb) > 1)
				dpawns[j]++;
			
			if (j == oturn)
				break;
		}
	}
	//Isolated, Rammed and Passed Pawns
	for (j = turn; true; j = oturn) {
		pwns = board.m_pbbs[BB_PAWNS][j];
		while (pwns) {	
			i = BB_NEXTBIT(pwns);
			
			//Position
			if (j == turn) {
				if (j == BB_WHITE)
					ptscore += m_wpawnpos[i];
				else
					ptscore += m_bpawnpos[i];
			}
			else {
				if (j == BB_WHITE)
					ptscore -= m_wpawnpos[i];
				else
					ptscore -= m_bpawnpos[i];
			}
			
			//Passed
			if (FILE(i) > 0 && FILE(i) < 7) {
				bb = board.get_pawn_column(1-j,FILE(i)-1);
				if (!pawnslower(board,bb,RANK(i),(j == BB_WHITE))) {
					bb = board.get_pawn_column(1-j,FILE(i));
					if (!pawnslower(board,bb,RANK(i),(j == BB_WHITE))) {
						bb = board.get_pawn_column(1-j,FILE(i)+1);
						if (!pawnslower(board,bb,RANK(i),(j == BB_WHITE)))
							ppawns[j]++;
					}
				}
			}
			else if (FILE(i) == 0) {
				bb = board.get_pawn_column(1-j,FILE(i));
				if (!pawnslower(board,bb,RANK(i),(j == BB_WHITE))) {
					bb = board.get_pawn_column(1-j,FILE(i)+1);
					if (!pawnslower(board,bb,RANK(i),(j == BB_WHITE)))
						ppawns[j]++;
				}
			}
			else if (FILE(i) == 7) {
				bb = board.get_pawn_column(1-j,FILE(i));
				if (!pawnslower(board,bb,RANK(i),j == BB_WHITE)) {
					bb = board.get_pawn_column(1-j,FILE(i)-1);
					if (!pawnslower(board,bb,RANK(i),(j == BB_WHITE)))
						ppawns[j]++;
				}
			}
			
			//Isolated
			if (FILE(i) > 0 && FILE(i) < 7) {
				if (board.get_pawn_column(j,FILE(i)-1) == (bitboard)0 &&
					board.get_pawn_column(j,FILE(i)+1) == (bitboard)0)
					ipawns[j]++;
			}
			else if (FILE(i) == 0) {
				if (board.get_pawn_column(j,FILE(i)+1) == (bitboard)0)
					ipawns[j]++;
			}
			else if (FILE(i) == 7) {
				if (board.get_pawn_column(j,FILE(i)-1) == (bitboard)0)
					ipawns[j]++;
			}
			
			//Rams
			if (j == BB_WHITE) {
				if (IS_SET(board.m_pbbs[BB_PAWNS][1-j],i+8) != (bitboard)0)
					rams[j]++;
			}
			else if (j == BB_BLACK) {
				if (IS_SET(board.m_pbbs[BB_PAWNS][1-j],i-8) != (bitboard)0)
					rams[j]++;
			}
			pwns = CLEAR_BIT(pwns,i);
		}
		if (j == oturn)
			break;
	}
	
	if (m_verbose) {
		cout << "DPAWNS: T: " << dpawns[board.turn()] << " O: " << dpawns[board.otherTurn()] << endl;
		cout << "PPAWNS T: " << ppawns[board.turn()] << " O: " << ppawns[board.otherTurn()] << endl;
		cout << "IPAWNS T: " << ipawns[board.turn()] << " O: " << ipawns[board.otherTurn()] << endl;
		cout << "RAMS: T: " << rams[board.turn()] << " O: " << rams[board.otherTurn()] << endl;
	}
	
	//DPAWNS: 30 IPAWNS: 20 PPAWNS: 50  
	score = ((dpawns[oturn]-dpawns[turn])*10 + 
		(ipawns[oturn]-ipawns[turn])*8 + 
		20*(ppawns[turn]-ppawns[oturn]));
	
		/*if (!(score == phe->m_score || score == -phe->m_score) && phe->m_pawns[board.turn()] == board.m_pawns[board.turn()] && 
		phe->m_pawns[board.otherTurn()] == board.m_pawns[board.otherTurn()]) 
		cout << "Major Problems With Pawns! S: " << score << " MS: " << phe->m_score 
	<< " T: " << board.turn() << " MT: " << phe->m_turn << endl;*/
	
	phe->m_score = score; phe->m_turn = turn; phe->m_ptscore = ptscore;
	phe->m_pawns[0] = board.m_pbbs[BB_PAWNS][0]; 
	phe->m_pawns[1] = board.m_pbbs[BB_PAWNS][1];
	
	return score;
}

int Evaluator::mobrook(Board& board, int pos, int turn) {
	
	int score=0;
	
	// Rook mobility is very important, also encourages rook play along open files
	// Max mobility score is 70
	score = Board::bb_count(board.get_rook_attacks(pos));

	// Specialized bonus for having a rook on the 7th rank
	// Max 7th rank bonus is 30
	if (turn == BB_WHITE && RANK(pos) == 7)
		score += 6;
	else if (turn == BB_BLACK && RANK(pos) == 1)
		score += 6;
	
	return score;
}

int Evaluator::mobbishop(Board& board, int pos) {
	return Board::bb_count(board.get_bishop_attacks(pos));
}

int Evaluator::mobknight(Board& board, int pos) {
	return Board::bb_count(board.get_knight_attacks(pos));
}

int Evaluator::mobqueen(Board& board, int pos) {
	return Board::bb_count(board.get_bishop_attacks(pos)) + Board::bb_count(board.get_rook_attacks(pos));
}

bool Evaluator::pawnslower(Board& board, bitboard bb, int rank, bool below) {
	
	int i;
	for (i = 0; i < 8; i++) {
		if (IS_SET(bb,i)) {
			if (below && rank < i) 	
				return true;
			else if (!below && rank > i)
				return true;
		}
	}
	return false;
}

// -EVAL_PCVAL(BB_PAWNS) <= score <= EVAL_PCVAL(BB_PAWNS)
int Evaluator::queens(Board& board) {
	int queens[2];
	int queen=0,turn = board.turn(), oturn = board.otherTurn();
	// Queen out too early?
	queens[turn] = BB_NEXTBIT(board.m_pbbs[BB_QUEEN][turn]);
	queens[oturn] = BB_NEXTBIT(board.m_pbbs[BB_QUEEN][oturn]);
	if (turn == BB_WHITE && queens[turn] > 0 && RANK(queens[turn]) > 2)
		queen -= 15;
	else if (turn == BB_BLACK && queens[turn] > 0 && RANK(queens[turn]) < 6)
		queen -= 15;
	else if (oturn == BB_WHITE && queens[oturn] > 0 && RANK(queens[oturn]) > 2)
		queen += 15;
	else if (oturn == BB_BLACK && queens[oturn] > 0 && RANK(queens[oturn]) < 6)
		queen += 15;
	
	return queen;
}

// -EVAL_PCVAL(BB_PAWNS) <= score <= EVAL_PCVAL(BB_PAWNS)
int Evaluator::kingsafety(Board& board) {
	
	
	return 0;
}

// Evaluates the castling advantage
// -EVAL_PCVAL(BB_PAWNS) <= score <= EVAL_PCVAL(BB_PAWNS)
int Evaluator::castle(Board& board) {
	
	int cscore=0;
	
	int cval = board.num_castles(board.turn())-board.num_castles(board.otherTurn());
	int cast = (int)board.is_castled(board.turn()) - (int)board.is_castled(board.otherTurn());
	
	if (cval > 0)
		cscore += 5;
	else if (cval < 0)
		cscore -= 5;
	
	if (cast > 0)
		cscore += 15;
	else if (cast < 0)
		cscore -= 15;
	
	return cscore;
}

void Evaluator::verbosity(int posscore,int castles,int ptable,int queen,int pawnscore,int ksafety,int tropism, int mob) {
	cout << "*** In Order of Importance ***" << endl << endl;
	cout << "-Positional Score: " << posscore << endl;
	cout << "\tCastle Score: " << castles << endl;
	cout << "\tPiece Table (Piece Placement Assessment): " << ptable << endl;
	cout << "\tQueen: " << queen << endl;
	cout << "\tPawn Score: " << pawnscore << endl;
	cout << "\tKing Safety: " << ksafety << endl;
	cout << "\tTropism: " << tropism << endl;
	cout << "\tMobility: " << mob << endl;
}

void Evaluator::verbose() {
	m_verbose = true;
}
