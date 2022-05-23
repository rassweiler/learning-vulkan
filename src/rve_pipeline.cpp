#include "../include/rve_pipeline.hpp"
#include "../include/rve_model.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cassert>

namespace rve {
	RvePipeline::RvePipeline(
		RveVulkanDevice& device,
		const std::string& vertFilePath,
		const std::string& fragFilePath,
		const RvePipelineConfigInfo& configInfo) : rveVulkanDevice{device} {
			CreateGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
	}

	RvePipeline::~RvePipeline() {
		vkDestroyShaderModule(rveVulkanDevice.Device(), vertShaderModule, nullptr);
		vkDestroyShaderModule(rveVulkanDevice.Device(), fragShaderModule, nullptr);
		vkDestroyPipeline(rveVulkanDevice.Device(), graphicsPipeline, nullptr);
	}

	std::vector<char> RvePipeline::ReadFile(const std::string& filePath) {
		std::ifstream fileStream{filePath, std::ios::ate | std::ios::binary};

		if(!fileStream.is_open()){
			throw std::runtime_error("(rve_pipeline.cpp) Failed to open file: " + filePath);
		}

		size_t fileSize = static_cast<size_t>(fileStream.tellg());
		std::vector<char> characterBuffer(fileSize);
		fileStream.seekg(0);
		fileStream.read(characterBuffer.data(), fileSize);
		fileStream.close();

		return characterBuffer;
	}

	void RvePipeline::CreateGraphicsPipeline(
		const std::string& vertFilePath,
		const std::string& fragFilePath,
		const RvePipelineConfigInfo& configInfo) {
			assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "(rve_pipeline.cpp) Error: No pipelineLayout provided in configInfo");
			assert(configInfo.renderPass != VK_NULL_HANDLE && "(rve_pipeline.cpp) Error: No renderPass provided in configInfo");

			auto vertCode = ReadFile(vertFilePath);
			auto fragCode = ReadFile(fragFilePath);

			CreateShaderModule(vertCode, &vertShaderModule);
			CreateShaderModule(fragCode, &fragShaderModule);

			VkPipelineShaderStageCreateInfo shaderStages[2];
			shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
			shaderStages[0].module = vertShaderModule;
			shaderStages[0].pName = "main";
			shaderStages[0].flags = 0;
			shaderStages[0].pNext = nullptr;
			shaderStages[0].pSpecializationInfo = nullptr;
			shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			shaderStages[1].module = fragShaderModule;
			shaderStages[1].pName = "main";
			shaderStages[1].flags = 0;
			shaderStages[1].pNext = nullptr;
			shaderStages[1].pSpecializationInfo = nullptr;

			auto bindingDescriptions = RveModel::Vertex::GetBindingDescriptions();
			auto attributeDescriptions = RveModel::Vertex::GetAttributeDescriptions();
			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
			vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

			VkPipelineViewportStateCreateInfo viewportInfo{};
			viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportInfo.viewportCount = 1;
			viewportInfo.pViewports = &configInfo.viewport;
			viewportInfo.scissorCount = 1;
			viewportInfo.pScissors = &configInfo.scissor;

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyCreateInfo;
			pipelineInfo.pViewportState = &viewportInfo;
			pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
			pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
			pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
			pipelineInfo.pDynamicState = nullptr;
			pipelineInfo.layout = configInfo.pipelineLayout;
			pipelineInfo.renderPass = configInfo.renderPass;
			pipelineInfo.subpass = configInfo.subpass;
			pipelineInfo.basePipelineIndex = -1;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

			if(vkCreateGraphicsPipelines(
				rveVulkanDevice.Device(),
				VK_NULL_HANDLE, 1,
				&pipelineInfo,
				nullptr,
				&graphicsPipeline) != VK_SUCCESS) {
					throw std::runtime_error("(rve_pipeline.cpp) Failed to create vulkan pipeline");
			}
	}

	void RvePipeline::CreateShaderModule(const std::vector<char>& shaderCode, VkShaderModule* shaderModule) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		if(vkCreateShaderModule(rveVulkanDevice.Device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("(rve_pipeline.cpp) Failed to create shader module");
		}
	}

	RvePipelineConfigInfo RvePipeline::DefaultPipelineConfigInfo(uint32_t width, uint32_t height) {
		RvePipelineConfigInfo configInfo{};

		configInfo.inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;

		configInfo.scissor.offset = {0, 0};
		configInfo.scissor.extent = {width, height};

		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;
		configInfo.multisampleInfo.pSampleMask = nullptr;
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;

		configInfo.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};
		configInfo.depthStencilInfo.back = {};

		return configInfo;
	}

	void RvePipeline::Bind(VkCommandBuffer commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}
} // namespace rve
