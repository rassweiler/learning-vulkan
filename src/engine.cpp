#include "../include/engine.hpp"

namespace rve {
	void Engine::Run() {
		while(!rveWindow.ShouldClose()) {
			glfwPollEvents();
		}
	}
}