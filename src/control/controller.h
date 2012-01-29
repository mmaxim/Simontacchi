//Mike Maxim
//Engine controller interface

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <globals.h>
#include <exception.h>
#include <board.h>
#include <player.h>
#include <configfile.h>

#define CONTROLLER_WINBOARD		0
#define CONTROLLER_UCI			1
#define CONTROLLER_MAX			2

class Controller {
public:

	static Controller* getInstance(int);

	//Init the controller
	virtual void init(ConfigFile*)=0;

	//Process a command from the host
	virtual void process_command(const string&)=0;
	
	//Get a pointer to the Player object
	virtual Player* getPlayer()=0;

	//Get a pointer to the Board object
	virtual Board* getBoard()=0;

	//Prompt
	virtual void prompt()=0;

protected:
	
	double allocate_time(Clock*, Clock*);


private:

	static Controller* m_controllers[CONTROLLER_MAX]; 
};

#endif //__CONTROLLER_H__
