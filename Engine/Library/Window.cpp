#include "Window.h"

namespace gll
{
	Window win;
	Window* window()
	{
		return &win;
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		win.width = width;
		win.height = height;
	}
	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		win.mousePos.x = (((xpos * 2.0f) / win.width) - 1.0f) * ((float)win.width / (float)win.height);
		win.mousePos.y = -(((ypos * 2.0f) / win.height) - 1.0f);
	}

	void Window::createWindow(std::string titles, unsigned int widths, unsigned int heights)
	{
		width = widths;
		height = heights;

        glfwInit(); srand(time(NULL));
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        monitor = glfwGetPrimaryMonitor();

		handel = glfwCreateWindow(width, height, titles.c_str(), NULL, NULL);
        if(handel == NULL)
            throw std::runtime_error("Vulkan Engine : Can not create window object!");

		glfwSetFramebufferSizeCallback(handel, framebuffer_size_callback);
		glfwSetCursorPosCallback(handel, mouse_callback);
	}
	void Window::createWindowSurface(Instance &instance)
    {
        if (glfwCreateWindowSurface(instance.instance, handel, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

	void Window::updateWindow()
	{
		glfwPollEvents();
	}
	void Window::deleteWindow()
	{
		glfwTerminate();
	}
    void Window::deleteWindowSurface(Instance &instance)
    {
        if( surface ) {
          vkDestroySurfaceKHR(instance.instance, surface, nullptr);
        }
    }

	bool Window::isActive()
	{
		return !glfwWindowShouldClose(handel);
	}

	void Window::close()
	{
		glfwSetWindowShouldClose(handel, true);
	}
	void Window::updateSize(glm::ivec2 newsize)
	{
		width = newsize.x;
		height = newsize.y;
	}
	void Window::updateMouseVisablity(bool can)
	{
		glfwSetInputMode(handel, GLFW_CURSOR, can ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}

	void Window::toggleFullscreen(Instance &instance, bool should)
    {
		if (should) {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(handel, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else {
			glfwSetWindowMonitor(handel, NULL, 100, 100, 800, 600, 0);
		}
    }
}
