#include "lve_window.hpp"

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
}