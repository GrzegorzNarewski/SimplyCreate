#include "Input.h"

namespace gll
{
	InputHandler ih;
	InputHandler* inputhandler()
	{
		return &ih;
	}

	void InputHandler::createHandler(Window* p_window)
	{
		window = p_window;
		for (int i = 0; i < 316; i++)
			keyPresses[i] = false;
		for (int i = 0; i < 7; i++)
			mouseKeyPresses[i] = false;
	}
	void InputHandler::updateHandler()
	{
        for (int i = 0; i < 7; i++) {
            if (glfwGetMouseButton(window->handel, i) == true && mouseKeyPresses[i] == false) {
                mouseKeyPresses[i] = true;
                mouseKeyStates[i] = KEY_STATES::KEY_PRESSED;
            }
            else if (glfwGetMouseButton(window->handel, i) == true && mouseKeyPresses[i] == true) {
                mouseKeyStates[i] = KEY_STATES::KEY_HELD;
            }
            else if (glfwGetMouseButton(window->handel, i) == false && mouseKeyPresses[i] == true) {
                mouseKeyPresses[i] = false;
                mouseKeyStates[i] = KEY_STATES::KEY_RELEASED;
            }
            else mouseKeyStates[i] = KEY_STATES::KEY_STEADY;
        }
	}

	KEY_STATES InputHandler::getKey(int key)
	{
		if (glfwGetKey(window->handel, key) == true && keyPresses[key - 32] == false) {
			keyPresses[key - 32] = true;
			return KEY_STATES::KEY_PRESSED;
		}
		else if (glfwGetKey(window->handel, key) == true && keyPresses[key - 32] == true) {
			return KEY_STATES::KEY_HELD;
		}
		else if (glfwGetKey(window->handel, key) == false && keyPresses[key - 32] == true) {
			keyPresses[key - 32] = false;
			return KEY_STATES::KEY_RELEASED;
		}
		return KEY_STATES::KEY_STEADY;
	}
	KEY_STATES InputHandler::getMouseKey(int key)
	{
		return mouseKeyStates[key];
	}
}