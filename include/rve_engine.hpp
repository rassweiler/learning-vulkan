#pragma once

#include "rve_pipeline.hpp"
#include "rve_window.hpp"
#include "rve_vulkan_device.hpp"

namespace rve {
	class RveEngine {
	private:
		RveWindow rveWindow{windowWidth, windowHeight, "Vulkan Test"};
		RveVulkanDevice rveVulkanDevice{rveWindow};
		RvePipeline rvePipeline{
			rveVulkanDevice,
			"../shaders/simple_shader.vert.spv",
			"../shaders/simple_shader.frag.spv",
			RvePipeline::DefaultPipelineConfigInfo(windowWidth, windowHeight)};

	public:
		void Run();

		static constexpr int windowWidth = 600;
		static constexpr int windowHeight = 600;
	};
} // namespace rve
