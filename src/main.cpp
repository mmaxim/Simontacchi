//Mike Maxim
//Entry

#include <control/engine.h>

int main() {

	//Initialize the engine
	Engine::getInstance()->init();

	//Transfer control over to the engine for the remainder of execution
	return Engine::getInstance()->main();
}
