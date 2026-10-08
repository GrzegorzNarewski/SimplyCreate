#include "AnimMesh.h"

namespace gll
{
	class EngineManager
	{
		void (*update)();

		std::map<std::string, Scene> scenes;

	public:
		
		void addObject(std::string scene, Object* object);
		void removeObject(std::string scene, std::string name);

		void createEngine(void update());
		void updateEngine();
		void deleteEngine();
	};

	EngineManager* engine();
}