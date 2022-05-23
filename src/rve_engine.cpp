#include "../include/rve_engine.hpp"

#include <stdexcept>
#include <array>

namespace rve {
	RveEngine::RveEngine() {
		LoadModels();
		CreatePipelineLayout();
		CreatePipeline();
		CreateCommandBuffers();
	}

	RveEngine::~RveEngine() {
		vkDestroyPipelineLayout(rveVulkanDevice.Device(), pipelineLayout, nullptr);
	}

	void RveEngine::CreatePipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if(vkCreatePipelineLayout(rveVulkanDevice.Device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to create pipeline layout");
		}
	}

	void RveEngine::CreatePipeline() {
		auto pipelineConfig = RvePipeline::DefaultPipelineConfigInfo(rveSwapChain.Width(), rveSwapChain.Height());
		pipelineConfig.renderPass = rveSwapChain.GetRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		rvePipeline = std::make_unique<RvePipeline>(
			rveVulkanDevice,
			"../shaders/simple_shader.vert.spv",
			"../shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}

	void RveEngine::CreateCommandBuffers() {
		commandBuffers.resize(rveSwapChain.ImageCount());

		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = rveVulkanDevice.GetCommandPool();
		allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if(vkAllocateCommandBuffers(rveVulkanDevice.Device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to create command buffer");
		}

		for (int i =0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("(rve_engine.cpp) Failed to start recording command buffer");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = rveSwapChain.GetRenderPass();
			renderPassInfo.framebuffer = rveSwapChain.GetFrameBuffer(i);
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = rveSwapChain.GetSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = {0.1f, 0.1f, 0.2f, 1.0f};
			clearValues[1].depthStencil = {1.0f, 0};

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			rvePipeline->Bind(commandBuffers[i]);
			rveModel->Bind(commandBuffers[i]);
			rveModel->Draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);

			if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("(rve_engine.cpp) Failed to end recording command buffer");
			}
		}
	}

	void RveEngine::DrawFrame() {
		uint32_t imageIndex;
		auto result = rveSwapChain.AcquireNextImage(&imageIndex);

		if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("(rve_engine.cpp) Failed to get swap chain image");
		}

		result = rveSwapChain.SubmitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

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

	void RveEngine::Run() {
		while(!rveWindow.ShouldClose()) {
			glfwPollEvents();
			DrawFrame();
		}
		vkDeviceWaitIdle(rveVulkanDevice.Device());
	}
} // namespace rve
