//Mike Maxim
//UCI controller interface

#ifndef __UCICONTROLLER_H__
#define __UCICONTROLLER_H__

#include <globals.h>
#include <controller.h>
#include <threadsvc.h>
#include <configfile.h>

#define UCI_INSTALL_PROC(name,f)	(m_proc_map[name] = &UCIController::f)

class UCIController : public Controller {
public:

	//Init the controller
	virtual void init(ConfigFile*);

	//Process a command from the host
	virtual void process_command(const string&);
	
	//Get a pointer to the Player object
	virtual Player* getPlayer();

	//Get a pointer to the Board object
	virtual Board* getBoard();

	//Prompt
	virtual void prompt();

protected:

	UCIController();

private:
	
	friend class Controller;
	friend void* uci_move_thread(void*);
	
	typedef bool (UCIController::*uci_proc_func)(const string&);
	map<string, uci_proc_func> m_proc_map;

	void start_thinking();

	Player m_player;
	Board m_board;
	ThreadService* m_thrsvc;
	ConfigFile* m_cfg;
	Clock m_meclock,m_opclock;
	thr_id m_thinkthread;
	bool m_ponderenabled,m_thinking;
	double m_timeallocated;

	//Command processors
	bool proc_uci(const string&);
	bool proc_position(const string&);
	bool proc_go(const string&);
	bool proc_setoption(const string&);
	bool proc_isready(const string&);
	bool proc_quit(const string&);
	bool proc_stop(const string&);
	bool proc_ponderhit(const string&);
};

#endif
