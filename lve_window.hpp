#pragma once
#define GLFW_INCLUDE_VULKAN

#include <glfw/glfw3.h>
#include <vulkan/vulkan.h>

#include <string>

namespace lve {
	class LveWindow
	{
	public:
		LveWindow(int width, int heigth, std::string name);
		~LveWindow();

		LveWindow(const LveWindow&) = delete;
		void operator=(const LveWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:

		void initWindow();

		const int width;
		const int heigth;

		std::string windowName;
		GLFWwindow* window;
	};
}