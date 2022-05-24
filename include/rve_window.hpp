#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>

namespace rve {
	class RveWindow {
	private:
		void Init();
		static void FrameBufferResizedCallback(GLFWwindow* window, int width, int height);

		int windowWidth;
		int windowHeight;
		bool frameBufferResized = false;
		std::string windowName;

		GLFWwindow *window;

	public:
		RveWindow(int width, int height, std::string name);
		~RveWindow();
		RveWindow(const RveWindow &) = delete;
		RveWindow &operator=(const RveWindow &) = delete;

		bool ShouldClose();
		bool isWindowResized();
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
		void ResetWindowResizedFlag();
		VkExtent2D GetExtent();
	};
} // namespace rve
