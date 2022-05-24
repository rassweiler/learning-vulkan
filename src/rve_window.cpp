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
		glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);

		window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window,FrameBufferResizedCallback);
	}

	void RveWindow::FrameBufferResizedCallback(GLFWwindow* window, int width, int height) {
		auto rveWindow = reinterpret_cast<RveWindow *>(glfwGetWindowUserPointer(window));
		rveWindow->frameBufferResized = true;
		rveWindow->windowWidth = width;
		rveWindow->windowHeight = height;
	}

	bool RveWindow::ShouldClose() {
		return glfwWindowShouldClose(window);
	}

	bool RveWindow::isWindowResized() {
		return frameBufferResized;
	}

	void RveWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("(rve_window.cpp) Failed to create window surface");
		}
	}

	void RveWindow::ResetWindowResizedFlag() {
		frameBufferResized = false;
	}

	VkExtent2D RveWindow::GetExtent() {
		return {static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight)};
	}
} // namespace rve
