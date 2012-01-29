//Mike Maxim
//Display manager singleton management

#include <displayman.h>
#include <origdisplayman.h>
#include <craftydisplayman.h>
#include <ucidisplayman.h>

DisplayManager* DisplayManager::m_instance[DISPLAY_MANAGER_MAX] = {NULL};

DisplayManager* DisplayManager::getInstance(int man) {
	
	if (m_instance[man] == NULL) {
		switch (man) {
		case DISPLAY_MANAGER_LEGACY:
			m_instance[man] = new LegacyDisplayManager();
			break;
		case DISPLAY_MANAGER_CRAFTY:
			m_instance[man] = new CraftyDisplayManager();
			break;
		case DISPLAY_MANAGER_UCI:
			m_instance[man] = new UCIDisplayManager();
			break;
		default:
			return NULL;
		};
	}
	
	return m_instance[man];
}
