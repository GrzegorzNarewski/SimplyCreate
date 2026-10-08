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

		scenes["default"].createScene();
	}
	void EngineManager::updateEngine()
	{
		dTime()->refresh();
		while (window()->isActive())
		{
			window()->updateWindow();

			inputhandler()->updateHandler();

			dTime()->updateDeltaTime();


			stock()->beginRender();

			stock()->beginRecordCommandBuffer("default", "default");
			update();
			rManager()->updateViewMatrices(activeCamera(), window()->width, window()->height);
			scenes["default"].updateScene();
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

	void EngineManager::addObject(std::string scene, Object* object)
	{
		scenes[scene].addObject(object->name(), object);
	}
	void EngineManager::removeObject(std::string scene, std::string name)
	{
		scenes[scene].removeObject(name);
	}
}