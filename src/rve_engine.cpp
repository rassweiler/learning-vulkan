#include "../include/rve_engine.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <array>

namespace rve {
	struct RveSimplePushConstantData {
		glm::mat2 tranform{1.0f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	RveEngine::RveEngine() {
		LoadGameObjects();
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

	void RveEngine::LoadGameObjects() {
		std::vector<RveModel::Vertex> vertices {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		auto rveModel = std::make_shared<RveModel>(rveVulkanDevice, vertices);
		auto triangle = RveGameObject::CreateGameObject();
		triangle.model = rveModel;
		triangle.color = {0.2f, 0.6f, 0.1f};
		triangle.transform2d.translation.x = 0.2f;
		triangle.transform2d.scale = {2.0f, 0.5f};
		triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();
		rveGameObjects.push_back(std::move(triangle));
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

		RenderGameObjects(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);

		if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("(rve_engine.cpp) Failed to end recording command buffer");
		}
	}

	void RveEngine::RenderGameObjects(VkCommandBuffer commandBuffer) {
		int i = 0;
		for(auto& object: rveGameObjects) {
			i += 1;
			object.transform2d.rotation = glm::mod<float>(
				object.transform2d.rotation + 0.001f * i, 2.0f * glm::pi<float>()
			);
		}
		rvePipeline->Bind(commandBuffer);
		for(auto& object: rveGameObjects) {
			object.transform2d.rotation = glm::mod(object.transform2d.rotation + 0.01f, glm::two_pi<float>());
			RveSimplePushConstantData push{};
			push.offset = object.transform2d.translation;
			push.color = object.color;
			push.tranform = object.transform2d.mat2();
			vkCmdPushConstants(
				commandBuffer, 
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
				0, 
				sizeof(RveSimplePushConstantData), 
				&push
			);
			object.model->Bind(commandBuffer);
			object.model->Draw(commandBuffer);
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
