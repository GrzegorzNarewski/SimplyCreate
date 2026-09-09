#include "EngineManager.h"

namespace gll
{
	EngineManager engi;
	EngineManager* engine()
	{
		return &engi;
	}

	void EngineManager::createEngine(void p_update())
	{
		update = p_update;

		stock()->createStock();

		window()->createWindow("Vulkan Engine", 800, 600);

		inputhandler()->createHandler(window());
	}
	void EngineManager::updateEngine()
	{
		while (window()->isActive())
		{
			window()->updateWindow();

			inputhandler()->updateHandler();

			update();
		}
	}
	void EngineManager::deleteEngine()
	{
		stock()->deleteStock();
		window()->deleteWindow();
	}
}