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
		void FreeCommandBuffers();
		void DrawFrame();
		void LoadModels();
		void RecreateSwapChain();
		void RecordCommandBuffer(int imageIndex);

		RveWindow rveWindow{windowWidth, windowHeight, "Vulkan Test"};
		RveVulkanDevice rveVulkanDevice{rveWindow};
		std::unique_ptr<RveSwapChain> rveSwapChain;
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
