#pragma once

#include "rve_pipeline.hpp" 
#include "rve_vulkan_device.hpp"
#include "rve_game_object.hpp"

#include <memory>
#include <vector>

namespace rve {
	class RveRenderSystem {
	public:
		RveRenderSystem(RveVulkanDevice& device, VkRenderPass renderPass);
		~RveRenderSystem();
		RveRenderSystem(const RveRenderSystem &) = delete;
		RveRenderSystem &operator=(const RveRenderSystem &) = delete;

		void RenderGameObjects(
			VkCommandBuffer commandBuffer, 
			std::vector<RveGameObject>& gameObjects
		);
	
	private:
		void CreatePipelineLayout();
		void CreatePipeline(VkRenderPass renderPass);

		RveVulkanDevice& rveVulkanDevice;
		std::unique_ptr<RvePipeline> rvePipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace rve
