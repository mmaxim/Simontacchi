//Mike Maxim
//Win32 thread implementation of the ThreadService interface

#ifndef __W32THREADSVC_H__
#define __W32THREADSVC_H__

#include <threadsvc.h>

class Win32ThreadService : public ThreadService {
public:

	virtual thr_id create(void* /* Function*/, void* /* Args */);
	virtual void cancel(thr_id);
	virtual void pause(thr_id);
	virtual void resume(thr_id);

protected:
	Win32ThreadService();
private:

	friend class ThreadService;

#ifdef WIN32
	vector<HANDLE> m_threads;
#endif
};

#endif
