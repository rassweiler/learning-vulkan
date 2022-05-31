#include "../include/rve_render_system.hpp"

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

	RveRenderSystem::RveRenderSystem(RveVulkanDevice& device, VkRenderPass renderPass) : 
		rveVulkanDevice{device}  {
			CreatePipelineLayout();
			CreatePipeline(renderPass);
	}

	RveRenderSystem::~RveRenderSystem() {
		vkDestroyPipelineLayout(rveVulkanDevice.Device(), pipelineLayout, nullptr);
	}

	void RveRenderSystem::CreatePipelineLayout() {
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

	void RveRenderSystem::CreatePipeline(VkRenderPass renderPass) {
		assert(
			pipelineLayout != nullptr &&
			"(rve_engine.cpp) Cannot create pipeline before pipeline layout"
		);

		RvePipelineConfigInfo pipelineConfig{};
		RvePipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		rvePipeline = std::make_unique<RvePipeline>(
			rveVulkanDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}	

	void RveRenderSystem::RenderGameObjects(
		VkCommandBuffer commandBuffer, 
		std::vector<RveGameObject>& gameObjects) {
			int i = 0;
			for(auto& object: gameObjects) {
				i += 1;
				object.transform2d.rotation = glm::mod<float>(
					object.transform2d.rotation + 0.001f * i, 2.0f * glm::pi<float>()
				);
			}
			rvePipeline->Bind(commandBuffer);
			for(auto& object: gameObjects) {
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
} // namespace rve
