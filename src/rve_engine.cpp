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
		CreatePipeline();
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
		assert(
			pipelineLayout != nullptr &&
			"(rve_engine.cpp) Cannot create pipeline before pipeline layout"
		);

		RvePipelineConfigInfo pipelineConfig{};
		RvePipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = rveRenderer.GetSwapChainRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		rvePipeline = std::make_unique<RvePipeline>(
			rveVulkanDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
		);
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
			
			if(auto commandBuffer = rveRenderer.BeginFrame()) {
				rveRenderer.BeginSwapChainRenderPass(commandBuffer);
				RenderGameObjects(commandBuffer);
				rveRenderer.EndSwapChainRenderPass(commandBuffer);
				rveRenderer.EndFrame();
			}
		}
		vkDeviceWaitIdle(rveVulkanDevice.Device());
	}
} // namespace rve
