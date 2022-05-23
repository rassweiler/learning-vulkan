#pragma once

#include "rve_pipeline.hpp"
#include "rve_window.hpp"
#include "rve_vulkan_device.hpp"
#include "rve_swap_chain.hpp"
#include "rve_model.hpp"

#include <memory>
#include <vector>

namespace rve {
	class RveEngine {
	private:
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void DrawFrame();
		void LoadModels();

		RveWindow rveWindow{windowWidth, windowHeight, "Vulkan Test"};
		RveVulkanDevice rveVulkanDevice{rveWindow};
		RveSwapChain rveSwapChain{rveVulkanDevice, rveWindow.GetExtent()};
		std::unique_ptr<RvePipeline> rvePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<RveModel> rveModel;

	public:
		RveEngine();
		~RveEngine();
		RveEngine(const RveEngine &) = delete;
		RveEngine &operator=(const RveEngine &) = delete;

		void Run();

		static constexpr int windowWidth = 600;
		static constexpr int windowHeight = 600;
	};
} // namespace rve
