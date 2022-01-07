#pragma once

#include <glfw/glfw3.h>
#include <vulkan.h>

#include <string>

namespace lve {
	class LveWindow
	{
	public:
		LveWindow(int width, int heigth, std::string name);
		~LveWindow();

		bool shouldClose() { return glfwWindowShouldClose(window); }
	private:

		void initWindow();

		const int width;
		const int heigth;

		std::string windowName;
		GLFWwindow* window;
	};
}