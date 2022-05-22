#pragma once

#include "../include/rve_window.hpp"
#include "../include/rve_pipeline.hpp"

namespace rve {
	class Engine {
	private:
		RveWindow rveWindow{windowWidth, windowHeight, "Vulkan Test"};
		RvePipeline rvePipeline{"../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv"};
		
	public:
		void Run();
		
		static constexpr int windowWidth = 600;
		static constexpr int windowHeight = 600;
	};
}