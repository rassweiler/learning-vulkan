#include "../include/rve_engine.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <stdexcept>
#include <array>

namespace rve {
	struct RveSimplePushConstantData {
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	RveEngine::RveEngine() {
		LoadModels();
		CreatePipelineLayout();
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	RveEngine::~RveEngine() {
		vkDestroyPipelineLayout(rveVulkanDevice.Device(), pipelineLayout, nullptr);
	}

	void RveEngine::CreatePipelineLayout() {
		VkPushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(RveSimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if(vkCreatePipelineLayout(rveVulkanDevice.Device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to create pipeline layout");
		}
	}

	void RveEngine::CreatePipeline() {
		RvePipelineConfigInfo pipelineConfig{};
		RvePipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = rveSwapChain->GetRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		rvePipeline = std::make_unique<RvePipeline>(
			rveVulkanDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}

	void RveEngine::CreateCommandBuffers() {
		commandBuffers.resize(rveSwapChain->ImageCount());

		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = rveVulkanDevice.GetCommandPool();
		allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if(vkAllocateCommandBuffers(rveVulkanDevice.Device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to create command buffer");
		}

		for (int i =0; i < commandBuffers.size(); i++) {
			
		}
	}

	void RveEngine::FreeCommandBuffers() {
		vkFreeCommandBuffers(rveVulkanDevice.Device(), 
			rveVulkanDevice.GetCommandPool(), 
			static_cast<uint32_t>(commandBuffers.size()), 
			commandBuffers.data()
		);
		commandBuffers.clear();
	}

	void RveEngine::DrawFrame() {
		uint32_t imageIndex;
		auto result = rveSwapChain->AcquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapChain();
			return;
		}

		if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("(rve_engine.cpp) Failed to get swap chain image");
		}

		RecordCommandBuffer(imageIndex);
		result = rveSwapChain->SubmitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || rveWindow.isWindowResized()) {
			rveWindow.ResetWindowResizedFlag();
			RecreateSwapChain();
			return;
		}

		if(result != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to show swap chain image");
		}
	}

	void RveEngine::LoadModels() {
		std::vector<RveModel::Vertex> vertices {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		rveModel = std::make_unique<RveModel>(rveVulkanDevice, vertices);
	}

	void RveEngine::RecreateSwapChain() {
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
		
		CreatePipeline();
	}
	
	void RveEngine::RecordCommandBuffer(int imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if(vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to start recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = rveSwapChain->GetRenderPass();
		renderPassInfo.framebuffer = rveSwapChain->GetFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = rveSwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {0.1f, 0.1f, 0.2f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(rveSwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(rveSwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{{0, 0}, rveSwapChain->GetSwapChainExtent()};
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		rvePipeline->Bind(commandBuffers[imageIndex]);
		rveModel->Bind(commandBuffers[imageIndex]);
		for(int i = 0; i < 4; i++) {
			RveSimplePushConstantData push{};
			push.offset = {0.0f, -0.4f + i * 0.25f};
			push.color = {0.0f, 0.0f, 0.2f + 0.2f * i};
			vkCmdPushConstants(
				commandBuffers[imageIndex], 
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
				0, 
				sizeof(RveSimplePushConstantData), 
				&push
			);
			rveModel->Draw(commandBuffers[imageIndex]);
		}

		vkCmdEndRenderPass(commandBuffers[imageIndex]);

		if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to end recording command buffer");
		}
	}

	void RveEngine::Run() {
		while(!rveWindow.ShouldClose()) {
			glfwPollEvents();
			DrawFrame();
		}
		vkDeviceWaitIdle(rveVulkanDevice.Device());
	}
} // namespace rve
