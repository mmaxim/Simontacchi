//Mike Maxim
//Engine Clock support

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <globals.h>
#include <strtoken.h>

#define CLOCK_SETTING_CONVENTIONAL		0
#define CLOCK_SETTING_ICS				1

typedef	short							clk_set;

class Clock {
public:

	Clock();
	
	void enable();
	void disable(); //Turn the clock off
	bool isEnabled() const;

	int getOriginalTime() const;
	int getIncrement() const;
	int getTimeLeft() const;

	//Set clock via Xboard "level" command
	bool parseClockString(const string&);

	//Time in either minutes or seconds
	void initClock(clk_set,int,int);
	void reset();

	//Time in seconds
	void setTimeLeft(int);
	void setIncrement(int);
	void setMovesLeft(int);

	void startClock();
	void stopClock(int);
	double getElapsedTime() const;

	bool timeLeft() const;

	string toString() const;

private:

	int parse_minsec(const string&);

	clk_set m_setting;
	int m_origtime,m_timeleft,m_increment,m_critmove; // In milliseconds
	double m_starttime;
	bool m_clockrunning,m_enabled;
};

#endif
