//Mike Maxim
//Win32 thread management

#include <w32threadsvc.h>

Win32ThreadService::Win32ThreadService() {

}

thr_id Win32ThreadService::create(void* func, void* args) {
	thr_id tid=0;
#ifdef WIN32
	DWORD dw;

	tid = (thr_id) m_threads.size();
	m_threads.push_back(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)func,args,0,&dw));
#endif

	return tid;
}

void Win32ThreadService::cancel(thr_id tid) {
#ifdef WIN32
	TerminateThread(m_threads[tid],0);
#endif
}

void Win32ThreadService::pause(thr_id tid) {
#ifdef WIN32
	SuspendThread(m_threads[tid]);
#endif
}

void Win32ThreadService::resume(thr_id tid) {
#ifdef WIN32
	ResumeThread(m_threads[tid]);
#endif
}

