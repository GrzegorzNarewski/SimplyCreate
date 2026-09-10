#include "Input.h"

namespace gll
{
	class EngineManager
	{
		void (*update)();

	public:

		void createEngine(void update());
		void updateEngine();
		void deleteEngine();
	};

	EngineManager* engine();
}