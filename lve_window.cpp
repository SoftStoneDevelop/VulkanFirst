#include "lve_window.hpp"

#include <stdexcept>

namespace lve {

	LveWindow::LveWindow(int width, int heigth, std::string name) : width{ width }, heigth{ heigth }, windowName{name} {
		initWindow();
	}

	LveWindow::~LveWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void LveWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(width, heigth, windowName.c_str(), nullptr, nullptr);
	}

	void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("faile to create window surface!");
		}
	}
}