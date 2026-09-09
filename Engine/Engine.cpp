#include "Library/EngineManager.h"

void update()
{
	if (gll::inputhandler()->getKey(GLFW_KEY_ESCAPE) == gll::KEY_PRESSED) {
		gll::window()->close();
	}
}

int main()
{
	gll::engine()->createEngine(update);
	gll::engine()->updateEngine();
	gll::engine()->deleteEngine();
}