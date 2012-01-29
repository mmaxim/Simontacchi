//Mike Maxim
//Clock implementation

#include <clock.h>

Clock::Clock() : m_clockrunning(false), m_enabled(true) {

}

void Clock::enable() {
	m_enabled = true;
}

void Clock::disable() {
	m_enabled = false;
}

bool Clock::isEnabled() const {
	return m_enabled;
}

int Clock::getOriginalTime() const {
	return m_origtime;
}

int Clock::getIncrement() const {
	return m_increment;
}

int Clock::getTimeLeft() const {
	return m_timeleft;
}

bool Clock::parseClockString(const string& str) {

	StringTokenizer tokens(str," ");
	string curtoken;

	m_clockrunning = false;
	if (tokens.size() == 0)
		return false;

	if (tokens.size() == 1)
		return false;

	curtoken = tokens[1];
	if (atoi(curtoken.c_str()) == 0) {
		m_setting = CLOCK_SETTING_ICS;
		
		if (tokens.size() == 2)
			return false;
		m_timeleft = parse_minsec(tokens[2]);
		if (tokens.size() == 3)
			return false;

		m_increment = atoi(tokens[3].c_str())*100;
		m_origtime = m_timeleft;

		return true;
	}
	else {
		m_critmove = atoi(curtoken.c_str());
		m_setting = CLOCK_SETTING_CONVENTIONAL;
		if (tokens.size() == 2)
			return false;

		curtoken = tokens[2];
		m_timeleft = parse_minsec(curtoken);
		m_origtime = m_timeleft;

		return true;
	}

	return true;
}

//Returns number of MS in the string
int Clock::parse_minsec(const string& str) {
	int mins=0,secs=0;

	if (strstr(str.c_str(),":")) {
		StringTokenizer tokens(str,":");
		mins = atoi(tokens[0].c_str());
		secs = atoi(tokens[1].c_str());
	}
	else 
		mins = atoi(str.c_str());

	return (mins*60*100 + secs*100);
}

void Clock::initClock(clk_set setting, int time, int aux) {

	m_setting = setting;
	m_clockrunning = false;
	if (m_setting == CLOCK_SETTING_CONVENTIONAL) {
		m_timeleft = time; m_origtime = m_timeleft; m_critmove = aux;
	}
	else {
		m_timeleft = time; m_origtime = m_timeleft; m_increment = aux;
	}
}

void Clock::reset() {
	m_timeleft = m_origtime;
	m_clockrunning = false;
}

void Clock::setTimeLeft(int timeleft) {
	m_timeleft = timeleft;

}

void Clock::setIncrement(int increment) {
	m_increment = increment;
}

void Clock::setMovesLeft(int movesleft) {
	m_critmove = movesleft;
}

void Clock::startClock() {
	m_starttime = getMS();
	m_clockrunning = true;
}

double Clock::getElapsedTime() const {
	if (m_clockrunning)
		return (getMS() - m_starttime);
	else
		return 0;
}

void Clock::stopClock(int move) {
	if (m_clockrunning) {
		m_timeleft -= ((int)(getMS() - m_starttime))*100;
		if (m_setting == CLOCK_SETTING_CONVENTIONAL && move >= m_critmove)
			m_timeleft = m_origtime;
	}	

	if (m_setting == CLOCK_SETTING_ICS)
		m_timeleft += m_increment;

	m_clockrunning = false;
}

bool Clock::timeLeft() const {
	if (m_enabled)
		return (m_timeleft >= 0);
	else
		return true;
}

string Clock::toString() const {
	char buffer[1024];
	int mins,secs;
	int tsecs;
	
	tsecs = (m_timeleft-(int)getElapsedTime()*100)/100;
	mins = tsecs/60;
	secs = tsecs%60;

	sprintf(buffer,"%0.2d:%0.2d",mins,secs);
	return string(buffer);
}
