//Mike Maxim
//Interface for controlling and using the Simontacchi chess engine

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <board.h>
#include <player.h>
#include <diag.h>
#include <bookgen.h>
#include <pgnloader.h>
#include <book.h>
#include <clock.h>
#include <configfile.h>
#include <controller.h>

#define ENGINE_CONFIG_FILE			"simon.cfg"

#define ENGINE_ENV_XBOARD			0
#define ENGINE_ENV_CONSOLE			1

typedef struct tagGAMELOGENTRY {
	int m_myrating,m_oprating,m_side;
	bool m_computer,m_saved;
	string m_name,m_event,m_result;

	vector<move_t> m_moves;
} GameLogEntry;	

class Engine {
public:

	static Engine* getInstance();

	void init();
	int main();

private:

	Engine();
	static Engine* m_instance;

	friend void* move_thread(void*);

	void createbook(const string&,const string&,int);
	void output_help();
	string version_str();

	int m_env;
	Controller* m_controller;
};

#endif
