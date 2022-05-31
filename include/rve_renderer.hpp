#pragma once

#include "rve_window.hpp"
#include "rve_vulkan_device.hpp"
#include "rve_swap_chain.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace rve {
	class RveRenderer {
	public:
		RveRenderer(RveWindow& window, RveVulkanDevice& device);
		~RveRenderer();
		RveRenderer(const RveRenderer &) = delete;
		RveRenderer &operator=(const RveRenderer &) = delete;

		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);
		bool IsFrameInProgress() const { return isFrameStarted; }
		VkRenderPass GetSwapChainRenderPass() const { return rveSwapChain->GetRenderPass(); }
		VkCommandBuffer GetCurrentCommandBuffer() const { 
			assert(isFrameStarted && 
			"(rve_renderer.hpp) Cannot get command buffer out of frame progress");
			return commandBuffers[currentFrameIndex]; 
		}
		int GetFrameIndex() const {
			assert(isFrameStarted && 
			"(rve_renderer.cpp) Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}
	
	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();

		RveWindow& rveWindow;
		RveVulkanDevice& rveVulkanDevice;
		std::unique_ptr<RveSwapChain> rveSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		uint32_t currentImageIndex;
		int currentFrameIndex{0};
		bool isFrameStarted{false};
	};
} // namespace rve
