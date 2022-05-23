#include "../include/rve_engine.hpp"

namespace rve {
	void RveEngine::Run() {
		while(!rveWindow.ShouldClose()) {
			glfwPollEvents();
		}
	}
} // namespace rve
