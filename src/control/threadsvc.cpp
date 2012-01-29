//Mike Maxim
//Thread service singleton management

#include <w32threadsvc.h>
#include <pththreadsvc.h>

ThreadService* ThreadService::m_services[THREAD_SERVICE_MAX] = {NULL};

ThreadService* ThreadService::getInstance(int svc) {
	if (m_services[svc] == NULL) {
		switch (svc) {
		case THREAD_SERVICE_WIN32:
			m_services[svc] = new Win32ThreadService();
			break;
        case THREAD_SERVICE_PTH:
            m_services[svc] = new PthThreadService();
            break;
		default:
			return NULL;
		};
	}

	return m_services[svc];
}
