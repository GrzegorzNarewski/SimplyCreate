#include "Library/EngineManager.h"

// camera and movement
gll::Camera camera;
const float SPEED = 2.0f;

// fps Counter
gll::Sprite fpsTextBg;
gll::Text fpsText;
float fpsTimer;
int fps;

// test box
gll::Mesh box;

void update()
{
	if (gll::inputhandler()->getKey(GLFW_KEY_ESCAPE) == gll::KEY_PRESSED) {
		gll::window()->close();
	}

	// calculate fps, and fps sprite transforms
	fps++;
	fpsTimer += gll::dTime()->deltaTime;
	if (fpsTimer >= 1.0f) {
		fpsTimer -= 1.0f;

		fpsText.context = "FPS: " + std::to_string(fps);
		fpsTextBg.transform.position.x = -1.0f + fpsText.getTextSize();
		fpsTextBg.transform.size = glm::vec3(fpsText.getTextSize(), 0.06f, 0.06f);

		fps = 0;
	}

	// camera movement
	glm::vec3 velocity = glm::vec3(0.0f);
	if (gll::inputhandler()->getKey(GLFW_KEY_W) == gll::KEY_HELD) {
		velocity += camera.Front * gll::dTime()->deltaTime * SPEED;
	}
	if (gll::inputhandler()->getKey(GLFW_KEY_S) == gll::KEY_HELD) {
		velocity -= camera.Front * gll::dTime()->deltaTime * SPEED;
	}
	if (gll::inputhandler()->getKey(GLFW_KEY_A) == gll::KEY_HELD) {
		velocity -= camera.Right * gll::dTime()->deltaTime * SPEED;
	}
	if (gll::inputhandler()->getKey(GLFW_KEY_D) == gll::KEY_HELD) {
		velocity += camera.Right * gll::dTime()->deltaTime * SPEED;
	}
	if (velocity != glm::vec3(0.0f)) {
		velocity = glm::normalize(velocity);
		camera.Position += velocity * 10.0f * gll::dTime()->deltaTime;
	}
}

int main()
{
	gll::engine()->createEngine(update);

	// set up camera
	camera.createCamera();
	gll::setActiveCamera(&camera);
	gll::window()->updateMouseVisablity(false);

	// fps counter
	fpsText.createText("fps_counter_text", "FPS: 0", "text", "Fonts/Font1.png", glm::vec3(-1.0f, -0.94f, 0.1f), glm::vec3(0.05f));
	fpsText.AlignX(-1.0f);
	fpsText.AlignType(gll::LEFT);
	gll::engine()->addObject("default", &fpsText);
	fpsTextBg.createSprite("fps_counter_background", "mesh2d", {"square.png"}, glm::vec3(-1.0f + fpsText.getTextSize(), -0.94f, 0.0f), glm::vec3(fpsText.getTextSize(), 0.06f, 0.06f), glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	fpsTextBg.AlignX(-1.0f);
	gll::engine()->addObject("default", &fpsTextBg);

	// test box
	box.createMesh("text_box", "mesh", "cube.obj", {"square.png", "emissionless.png"});
	gll::engine()->addObject("default", &box);

	gll::engine()->updateEngine();
	gll::engine()->deleteEngine();
}