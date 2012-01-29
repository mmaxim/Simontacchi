//Mike Maxim
//Winboard controller module

#ifndef __WINBOARDCONTROLLER_H__
#define __WINBOARDCONTROLLER_H__

#include <controller.h>
#include <engine.h>
#include <threadsvc.h>
#include <stack>

#define WBC_MODE_MATCH				0
#define WBC_MODE_FORCE				1
#define WBC_MODE_ANALYZE			2

#define WBC_FLAGS_BOOKENABLED		0
#define WBC_FLAGS_PONDERENABLED		1
#define WBC_FLAGS_MOVING			2
#define WBC_FLAGS_PONDERING			3
#define WBC_FLAGS_GUIMODE			4
#define WBC_FLAGS_MAX				5

#define WBC_INSTALL_PROC(name,f)	(m_proc_map[name] = &WinboardController::f)

class WinboardController : public Controller {
public:

	virtual void init(ConfigFile*);
	virtual void process_command(const string&);

	virtual Player* getPlayer();
	virtual Board* getBoard();

	virtual void prompt();

protected:
		
	WinboardController();

private:

	friend class Controller;
	friend void* wbc_move_thread(void*);

	typedef bool (WinboardController::*wbc_proc_func)(const vector<string>&);
	map<string, wbc_proc_func> m_proc_map;

	void log_game(GameLogEntry&);
	void log_state(move_t);
	bool kill_pondering(move_t,bool);
	bool apply_move(move_t,const string&);
	void apply_and_think(move_t);
	void start_move_thread();
	void start_ponder_thread(move_t);
	void output_clocks();

	//Engine state
	Player m_player;
	Board m_board,m_pboard;
	int m_mode;
	Clock m_clocks[2];
	Clock* m_meclock, *m_opclock;
	thr_id m_thinkthread,m_ponderthread;
	GameLogEntry m_curgame;
	bool m_flags[WBC_FLAGS_MAX];
	string m_logfile;
	stack<Board> m_undostack;
	ThreadService* m_thrsvc;
	Evaluator m_eval;
	move_t m_pondermove;
	PGNLoader m_pgnloader;

	//Command processors
	bool proc_new(const vector<string>&);
	bool proc_quit(const vector<string>&);
	bool proc_move(const vector<string>&);
	bool proc_xboard(const vector<string>&);
	bool proc_hard(const vector<string>&);
	bool proc_easy(const vector<string>&);
	bool proc_go(const vector<string>&);
	bool proc_result(const vector<string>&);
	bool proc_time(const vector<string>&);
	bool proc_otime(const vector<string>&);
	bool proc_force(const vector<string>&);
	bool proc_rating(const vector<string>&);
	bool proc_computer(const vector<string>&);
	bool proc_name(const vector<string>&);
	bool proc_protover(const vector<string>&);
	bool proc_ics(const vector<string>&);
	bool proc_accepted(const vector<string>&);
	bool proc_random(const vector<string>&);
	bool proc_level(const vector<string>&);
	bool proc_hint(const vector<string>&);
	bool proc_bk(const vector<string>&);
	bool proc_post(const vector<string>&);
	bool proc_nopost(const vector<string>&);
	bool proc_load(const vector<string>&);
	bool proc_playloaded(const vector<string>&);
	bool proc_clocks(const vector<string>&);
	bool proc_tofen(const vector<string>&);
	bool proc_setboard(const vector<string>&);
	bool proc_genmoves(const vector<string>&);
	bool proc_eval(const vector<string>&);
	bool proc_analyze(const vector<string>&);
	bool proc_dot(const vector<string>&);
	bool proc_exit(const vector<string>&);
	bool proc_gencapmoves(const vector<string>&);
	bool proc_bkon(const vector<string>&);
	bool proc_bkoff(const vector<string>&);
	bool proc_sd(const vector<string>&);
	bool proc_show(const vector<string>&);
	bool proc_undo(const vector<string>&);
	bool proc_remove(const vector<string>&);
	bool proc_playother(const vector<string>&);
	bool proc_movenow(const vector<string>&);
	bool proc_config(const vector<string>&);
	bool proc_ttlookup(const vector<string>&);
	bool proc_see(const vector<string>&);
	bool proc_movegenbench(const vector<string>&);
	bool proc_gennoncap(const vector<string>&);
	bool proc_cleartrans(const vector<string>&);
};

#endif //__WINBOARDCONTROLLER_H__
