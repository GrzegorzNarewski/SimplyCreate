#include "Scene.h"

namespace gll
{
	void Scene::createScene()
	{
		objects.clear();
	}
	void Scene::updateScene()
	{
		if (refreshObjects)
		{
			refreshObjects = false;
			tmpobjects.clear();
			for (auto x : objects) {
				if (x.second->getRenderPriority() > 0.0f)
					tmpobjects.push_back(x.second);
			}
			std::sort(tmpobjects.begin(), tmpobjects.end(), [](Object* a, Object* b)
				{
					return a->getRenderPriority() < b->getRenderPriority();
				});
		}

		for (int i = 0; i < tmpobjects.size(); i++)
		{
			tmpobjects[i]->updateObject("default");
		}
	}

	void Scene::addObject(std::string name, Object* object)
	{
		objects[name] = object;
		refreshObjects = true;
	}
	void Scene::removeObject(std::string name)
	{
		objects.erase(name);
		refreshObjects = true;
	}
};