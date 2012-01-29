//Mike Maxim
//Interface for a (possibly) platform-independent threading service

#ifndef __THREADSVC_H__
#define __THREADSVC_H__

#include <globals.h>
#include <player.h>

#define THREAD_SERVICE_WIN32		0
#define THREAD_SERVICE_PTH          1
#define THREAD_SERVICE_MAX			2

typedef int		thr_id;

class ThreadService {
public:

	static ThreadService* getInstance(int);

	virtual thr_id create(void* /* Function*/, void* /* Args */)=0;
	virtual void cancel(thr_id)=0;
	virtual void pause(thr_id)=0;
	virtual void resume(thr_id)=0;

    virtual void setPlayer(Player* player) { m_player = player; }

protected:

    Player* m_player;
    
private:

	static ThreadService* m_services[THREAD_SERVICE_MAX];

};

#endif
