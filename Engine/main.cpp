#include "Library/EngineManager.h"

// camera and movement
gll::Camera camera;
const float SPEED = 2.0f;

std::string x = "This is Perferct Hubert";
void update()
{
	if (gll::inputhandler()->getKey(GLFW_KEY_ESCAPE) == gll::KEY_PRESSED) {
		gll::window()->close();
	}

	//gll::rManager()->renderSprite("mesh2d", "square", "square.png", glm::vec3(0.0f), glm::vec3(0.5f), glm::vec3(0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	if (gll::inputhandler()->getKey(GLFW_KEY_W) == gll::KEY_HELD) {
		camera.Position += camera.Front * SPEED * gll::dTime()->deltaTime;
	}
	if (gll::inputhandler()->getKey(GLFW_KEY_S) == gll::KEY_HELD) {
		camera.Position -= camera.Front * SPEED * gll::dTime()->deltaTime;
	}
	if (gll::inputhandler()->getKey(GLFW_KEY_D) == gll::KEY_HELD) {
		camera.Position += camera.Right * SPEED * gll::dTime()->deltaTime;
	}
	if (gll::inputhandler()->getKey(GLFW_KEY_A) == gll::KEY_HELD) {
		camera.Position -= camera.Right * SPEED * gll::dTime()->deltaTime;
	}

	if (gll::inputhandler()->getKey(GLFW_KEY_UP) == gll::KEY_PRESSED) {
		x[0]++;
	}

	gll::Transform t; t.position = glm::vec3(-0.9f, 0.0f, 0.0f); t.size = glm::vec3(0.1f);
	gll::rManager()->renderText(x, t, glm::vec4(1.0f), gll::CENTERED, false);
	gll::rManager()->renderMesh("mesh", "Human_Forester.obj", { "Human.png", "emissionless.png" }, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.15f), glm::vec3(0.0f), glm::vec4(1.0f));
}

int main()
{
	gll::engine()->createEngine(update);

	// set up camera
	camera.createCamera();
	gll::setActiveCamera(&camera);
	gll::window()->updateMouseVisablity(false);


	gll::engine()->updateEngine();
	gll::engine()->deleteEngine();
}