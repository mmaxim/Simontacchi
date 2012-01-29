//Mike Maxim
//Pthreads threads service

#ifndef __PTHTHREADSVC_H__
#define __PTHTHREADSVC_H__

#include <threadsvc.h>
#include <pthread.h>
#include <map>

using namespace std;

class PthThreadService : public ThreadService {
public:

    virtual thr_id create(void*, void*);
	virtual void cancel(thr_id);
	virtual void pause(thr_id);
	virtual void resume(thr_id);

protected:
	PthThreadService();

private:

    friend class ThreadService;

    thr_id m_nextid;
    map<int, pthread_t> m_thrmap;
};

#endif
