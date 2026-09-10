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

		inputhandler()->createHandler(window());
	}
	void EngineManager::updateEngine()
	{
		while (window()->isActive())
		{
			window()->updateWindow();

			inputhandler()->updateHandler();

			stock()->beginRender();

			stock()->beginRecordCommandBuffer("default", "default");
			update();
			stock()->endRecordCommandBuffer();

			stock()->submitRender(true);
		}
	}
	void EngineManager::deleteEngine()
	{
		stock()->deleteStock();
	}
}