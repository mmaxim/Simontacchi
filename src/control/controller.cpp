//Mike Maxim
//Controller

#ifdef WIN32
#pragma warning (disable: 4180)
#endif

#include <controller.h>
#include <winboardcontroller.h>
#include <ucicontroller.h>

Controller* Controller::m_controllers[CONTROLLER_MAX] = {NULL};

Controller* Controller::getInstance(int controller) {
	if (m_controllers[controller] == NULL) {
		switch (controller) {
		case CONTROLLER_WINBOARD:
			m_controllers[controller] = new WinboardController();
			break;
		case CONTROLLER_UCI:
			m_controllers[controller] = new UCIController();
		};
	}
	return m_controllers[controller];
}

//Allocate appropriate amount of time for a move
double Controller::allocate_time(Clock* mclock, Clock* oclock) {

	double movespeed = 30.0;
	int gametime,increment,meclock,opclock;

	gametime = mclock->getOriginalTime(); increment = mclock->getIncrement();
	meclock = mclock->getTimeLeft(); opclock = oclock->getTimeLeft();

	// Check to see if we need to pick up the pace
	if ((gametime <= 5000 && increment == 0) || (meclock < gametime*4.0/5.0)) {
		if ((opclock-meclock) > (opclock/5.0))
			movespeed = 50.0;
		else if ((opclock-meclock) > (opclock/10.0))
			movespeed = 40.0;
		else if ((opclock-meclock) > (opclock/20.0))
			movespeed = 35.0;
	}

	// See if we can chill for a little and think things through
	if ((meclock-opclock) > (meclock/5.0))
		movespeed -= 15;
	else if ((meclock-opclock) > (meclock/10.0))
		movespeed -= 10;
	else if ((meclock-opclock) > (meclock/20.0))
		movespeed -= 5;

	// Need to cook
	if (meclock <= 1000 && increment < 300)
		movespeed += 30.0;

	return meclock/movespeed;
}
