#include "Library/EngineManager.h"

void update()
{
	if (gll::inputhandler()->getKey(GLFW_KEY_ESCAPE) == gll::KEY_PRESSED) {
		gll::window()->close();
	}

	if (gll::inputhandler()->getKey(GLFW_KEY_W) == gll::KEY_HELD) {
		gll::rManager()->renderSprite("mesh2d", "square", "square.png", glm::vec3(0.0f), glm::vec3(0.5f), glm::vec3(0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	
	gll::rManager()->renderMesh("mesh", "cube.obj", { "square.png", "emissionless.png" }, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, glfwGetTime() * 15.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

int main()
{
	gll::engine()->createEngine(update);
	gll::engine()->updateEngine();
	gll::engine()->deleteEngine();
}