#pragma once

#include "rve_vulkan_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace rve {
	class RveModel {
	public:
		struct Vertex {
			glm::vec2 position;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		RveModel(RveVulkanDevice& device, std::vector<Vertex> &vertices);
		~RveModel();
		RveModel(const RveModel &) = delete;
		RveModel &operator=(const RveModel &) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);
		
	private:
		void CreateVertexBuffers(std::vector<Vertex> &vertices);

		RveVulkanDevice& rveDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
} // namespace rve
