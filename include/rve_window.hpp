#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>

namespace rve {
	class RveWindow {
	private:
		void Init();

		const int windowWidth;
		const int windowHeight;

		std::string windowName;

		GLFWwindow *window;

	public:
		RveWindow(int width, int height, std::string name);
		~RveWindow();
		RveWindow(const RveWindow &) = delete;
		RveWindow &operator=(const RveWindow &) = delete;

		bool ShouldClose();
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
	};
} // namespace rve
