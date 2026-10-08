#include "Collider.h"

namespace gll
{
	class Object
	{
	protected:
		std::string obj_name;

		std::map<std::string, void*> ubo_addons;

	public:
		virtual void updateObject(std::string renderPass) = 0;
		virtual std::string type() = 0;
		virtual std::string name() = 0;
		virtual float getRenderPriority() = 0; 

		void addUBO(std::string name, void* ubo_add)
		{
			ubo_addons[name] = ubo_add;
		}
	};

	class Scene
	{
		std::map<std::string, Object*> objects;
		std::vector<Object*> tmpobjects;

		bool refreshObjects;

	public:

		void createScene();
		void updateScene();

		void addObject(std::string name, Object* object);
		void removeObject(std::string name);
	};
}