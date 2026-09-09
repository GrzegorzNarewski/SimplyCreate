#include "Instance.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

namespace gll
{
	class Window
	{
	public:
		GLFWwindow* handel;
        GLFWmonitor* monitor;
        VkSurfaceKHR surface;
        glm::vec2 mousePos;
		unsigned int width, height;

		void createWindow(std::string title, unsigned int width, unsigned int height);
		void updateWindow();
		void deleteWindow();

		void createWindowSurface(Instance &instance);
		void deleteWindowSurface(Instance &instance);

		bool isActive();

		void close();
		void updateSize(glm::ivec2 newsize);
		void updateMouseVisablity(bool can);
		void toggleFullscreen(Instance &instance, bool should);
	};

	Window* window();
}

