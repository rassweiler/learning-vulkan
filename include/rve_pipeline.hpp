#pragma once

#include "rve_vulkan_device.hpp"

#include <string>
#include <vector>

namespace rve {
	struct RvePipelineConfigInfo {
		uint32_t subpass = 0;
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
	};

	class RvePipeline {
	private:
		static std::vector<char> ReadFile(const std::string& filePath);
		void CreateGraphicsPipeline(
			const std::string& vertFilePath,
			const std::string& fragFilePath,
			const RvePipelineConfigInfo& configInfo);
		void CreateShaderModule(const std::vector<char>& shaderCode, VkShaderModule* shaderModule);

		RveVulkanDevice& rveVulkanDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

	public:
		RvePipeline(
			RveVulkanDevice& device,
			const std::string& vertFilePath,
			const std::string& fragFilePath,
			const RvePipelineConfigInfo& configInfo);
		~RvePipeline();
		RvePipeline(const RvePipeline &) = delete;
		RvePipeline &operator=(const RvePipeline &) = delete;

		static RvePipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);
		void Bind(VkCommandBuffer commandBuffer);
	};
} // namespace rve
