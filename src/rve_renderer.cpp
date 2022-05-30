#include "../include/rve_renderer.hpp"

#include <stdexcept>
#include <array>
#include <cassert>

namespace rve {
	RveRenderer::RveRenderer(RveWindow& window, RveVulkanDevice& device) : 
		rveVulkanDevice{device}, rveWindow{window} {
			RecreateSwapChain();
			CreateCommandBuffers();
	}

	RveRenderer::~RveRenderer() {
		FreeCommandBuffers();
	}

	VkCommandBuffer RveRenderer::BeginFrame() {
		assert(!isFrameStarted && "(rve_renderer.cpp) Cannot start frame while in progress");
		auto result = rveSwapChain->AcquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapChain();
			return nullptr;
		}

		if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("(rve_engine.cpp) Failed to get swap chain image");
		}
		
		isFrameStarted = true;
		auto commandBuffer = GetCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to start recording command buffer");
		}

		return commandBuffer;
	}

	void RveRenderer::EndFrame() {
		assert(isFrameStarted && "(rve_renderer.cpp) Cannot end frame while not in progress");
		auto commandBuffer = GetCurrentCommandBuffer();
		if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to end recording command buffer");
		}
		auto result = rveSwapChain->SubmitCommandBuffers(&commandBuffer, &currentImageIndex);

		if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || rveWindow.isWindowResized()) {
			rveWindow.ResetWindowResizedFlag();
			RecreateSwapChain();
		}

		if(result != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to show swap chain image");
		}

		isFrameStarted = false;
	}

	void RveRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(
			isFrameStarted && 
			"(rve_renderer.cpp) Cannot begin render pass while not in progress"
		);
		assert(
			commandBuffer == GetCurrentCommandBuffer() && 
			"(rve_renderer.cpp) Cannot begin render pass on buffer for another frame"
		);
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = rveSwapChain->GetRenderPass();
		renderPassInfo.framebuffer = rveSwapChain->GetFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = rveSwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {0.1f, 0.1f, 0.2f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(rveSwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(rveSwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{{0, 0}, rveSwapChain->GetSwapChainExtent()};
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void RveRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(
			isFrameStarted && 
			"(rve_renderer.cpp) Cannot end render pass while not in progress"
		);
		assert(
			commandBuffer == GetCurrentCommandBuffer() && 
			"(rve_renderer.cpp) Cannot end render pass on buffer for another frame"
		);
		vkCmdEndRenderPass(commandBuffer);
	}

	void RveRenderer::CreateCommandBuffers() {
		commandBuffers.resize(rveSwapChain->ImageCount());

		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = rveVulkanDevice.GetCommandPool();
		allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if(vkAllocateCommandBuffers(rveVulkanDevice.Device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to create command buffer");
		}
	}

	void RveRenderer::FreeCommandBuffers() {
		vkFreeCommandBuffers(rveVulkanDevice.Device(), 
			rveVulkanDevice.GetCommandPool(), 
			static_cast<uint32_t>(commandBuffers.size()), 
			commandBuffers.data()
		);
		commandBuffers.clear();
	}

	void RveRenderer::RecreateSwapChain() {
		auto extend = rveWindow.GetExtent();
		while (extend.width == 0 || extend.height == 0) {
			extend = rveWindow.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(rveVulkanDevice.Device());
		rveSwapChain = nullptr;

		if(rveSwapChain == nullptr) {
			rveSwapChain = std::make_unique<RveSwapChain>(rveVulkanDevice, extend);
		} else {
			rveSwapChain = std::make_unique<RveSwapChain>(rveVulkanDevice, extend, std::move(rveSwapChain));
			if(rveSwapChain->ImageCount() != commandBuffers.size()) {
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}
	}
} // namespace rve
