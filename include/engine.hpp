#pragma once

#include "../include/rve_window.hpp"

namespace rve {
	class Engine {
	private:
		RveWindow rveWindow{windowWidth, windowHeight, "Vulkan Test"};
		
	public:
		void Run();
		
		static constexpr int windowWidth = 600;
		static constexpr int windowHeight = 600;
	};
}