#include "../include/rve_window.hpp"

#include <stdexcept>

namespace rve {
	RveWindow::RveWindow(int width, int height, std::string name) : windowWidth{width}, windowHeight{height}, windowName{name} {
		Init();
	}
	RveWindow::~RveWindow(){
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	void RveWindow::Init() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

		window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);
	}

	bool RveWindow::ShouldClose() {
		return glfwWindowShouldClose(window);
	}

	void RveWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("(rve_window.cpp) Failed to create window surface");
		}
	}
} // namespace rve
