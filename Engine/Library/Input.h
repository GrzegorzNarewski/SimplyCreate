#include "FileManager.h"

namespace gll
{
	enum KEY_STATES
	{
		KEY_STEADY = 0,   // key is not pressed
		KEY_PRESSED = 1,   // key has been pressed
		KEY_HELD = 2,     // ket is being pressed
		KEY_RELEASED = 3,  // key has been released
	};

	class InputHandler
	{
		Window* window;

		bool keyPresses[316];

		bool mouseKeyPresses[7];
		KEY_STATES mouseKeyStates[7];

	public:

		void createHandler(Window* p_window);
		void updateHandler();

		// getters
		KEY_STATES getKey(int key);
		KEY_STATES getMouseKey(int key);
	};

	InputHandler* inputhandler();
}
