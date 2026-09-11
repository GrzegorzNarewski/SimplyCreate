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

		rManager()->createResourceManager();

		inputhandler()->createHandler(window());
	}
	void EngineManager::updateEngine()
	{
		Camera camera;
		camera.createCamera();
		while (window()->isActive())
		{
			window()->updateWindow();

			inputhandler()->updateHandler();

			stock()->beginRender();

			rManager()->updateViewMatrices(&camera, window()->width, window()->height);
			stock()->beginRecordCommandBuffer("default", "default");
			update();
			stock()->endRecordCommandBuffer();

			stock()->submitRender(true);

			rManager()->restartUBOs();
		}
	}
	void EngineManager::deleteEngine()
	{
		stock()->WaitDeviceIdle();
		rManager()->deleteResourceManager();
		stock()->deleteStock();
	}
}