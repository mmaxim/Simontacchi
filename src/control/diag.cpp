//Mike Maxim
//Some obsolete diagnostics

#include <diag.h>

int perft(Board&,int,int,int,int&);

void test_perft() {
	double st,et;
	int depth=7,nodes=0;
	Board board;

	board.fromFEN("r1b1k1nr/ppqp1ppp/2n1p3/2b3B1/3NP3/2N5/PPP2PPP/R2QKB1R w KQkq - 12 6");
	st=getMS();
	perft(board,depth,-S_INFINITY,S_INFINITY,nodes);
	et=getMS();

	cout << "Nodes: " << nodes << " S: " << et-st << " N/S: " << (int)(double(nodes)/(et-st)) << endl;
}

int perft(Board& board, int depth, int alpha, int beta, int& nodes) {

	MoveList moves;
	Evaluator eval;
	Board sboard;
	int i,score;
	bool incheck;

	nodes++;
	if (depth == 0)
		return eval.eval(board,alpha,beta);

	sboard = board;
	incheck = board.check();
	board.generate_moves(moves);
	for (i = 0; i < moves.size(); i++) {
		board.move(moves.get(i));

		score = perft(board,depth-1,-beta,-alpha,nodes);
		board = sboard;
		if (score >= beta)
			return beta;
		if (score > alpha) {
			alpha = score;
		}
	}
	return alpha;
}

void test_eval() {
	int i,numm=500000;
	Board board;
	Evaluator eval;
	double et,st;

	board.fromFEN("r1b1k1nr/ppqp1ppp/2n1p3/2b3B1/3NP3/2N5/PPP2PPP/R2QKB1R w KQkq - 12 6");
	st= getMS();
	for (i = 0; i < numm; i++) {
		eval.eval(board,-S_INFINITY,S_INFINITY);
	}
	et = getMS();
	cout << "Cycles: " << numm << " Time: " << et-st << " N/S: " << int(numm/(et-st)) << endl;
}

void test_make() {
	int i,numm=60000000;
	Board board,sboard;
	MoveList moves;
	double et,st;

	board.fromFEN("r3k2r/pb1n1pbp/4p1p1/q1n1P3/Bp1N1N2/4B3/PP3PPP/R2Q1RK1 w kq - 0 0");
	st= getMS();
	sboard = board;
	board.generate_moves(moves);
	for (i = 0; i < numm; i++) {
		board.move(moves.get(rand()%moves.size()));
		board = sboard;
	}
	et = getMS();
	cout << "Cycles: " << numm << " Time: " << et-st << " N/S: " << int(numm/(et-st)) << endl;
}

void test_movegen() {
	
	int i,numm=500000,nmoves=0;
	Board board;
	MoveList moves;
	double et,st;
	
	board.fromFEN("rnbqkb1r/1p3ppp/p2p1n2/4p1B1/3NP3/2N5/PPP2PPP/R2QKB1R w KQkq e5 12 6");
	st= getMS();
	for (i = 0; i < numm; i++) {
		board.generate_moves(moves);
		nmoves += moves.size();
		moves.clear();
	}
	et = getMS();
	cout << "Cycles: " << numm << " Moves: " << nmoves << " Time: " << et-st << " N/S: " << int(numm/(et-st)) << endl;
	moves.clear();
	st = getMS(); nmoves = 0;
	for (i = 0; i < numm; i++) {
		board.generate_capture_moves(moves);
		nmoves += moves.size();
		moves.clear();
	}
	et = getMS();
	cout << "Cycles: " << numm << " Captures: " << nmoves << " Time: " << et-st << " N/S: " << int(numm/(et-st)) << endl;
}

void test_zobrist() {
	
	int i,cycles=10,maxit=100000,it,errors=0,zequal,equal,terrors=0,positions=0;
	Board board,oboard;
	MoveList moves,omoves;
	
	for (i = 0; i < cycles; i++) {
		board.newGame(); oboard.newGame();
		srand((unsigned int)time(NULL)); errors = 0; zequal = 0; equal = 0; it=0;
		while (true && it < maxit) {
			board.generate_moves(moves);
			oboard.generate_moves(omoves);
			
			if (moves.size() == 0 && omoves.size() == 0)
				break;

			if (moves.size() > 0)
				board.move(moves.get(rand()%moves.size()));
			if (omoves.size() > 0)
				oboard.move(omoves.get(rand()%omoves.size()));
			
			if (board.get_signature() == oboard.get_signature())
				zequal++;
			if (board == oboard)
				equal++;

			if (board.get_signature() == oboard.get_signature() && !(board == oboard))
				errors++;
			
			moves.clear(); omoves.clear(); it++; positions++;
		}
	//	cout << board.toString() << endl;
		terrors += errors;
		cout << "Cycle: " << i+1 << " Errors: " << errors << " Equal: " << equal << " Zobrist Equal: " << zequal << endl;
	}
	
	cout << "Zobrist Test: Cycles: " << cycles << " Errors: " << terrors << " Positions: " << positions << endl;
}
