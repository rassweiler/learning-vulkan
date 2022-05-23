#include "../include/rve_model.hpp"

#include <cassert>
#include <cstddef>
#include <cstring>

namespace rve {
	RveModel::RveModel(RveVulkanDevice& device, std::vector<Vertex> &vertices) : rveDevice{device} {
		CreateVertexBuffers(vertices);
	}

	RveModel::~RveModel() {
		vkDestroyBuffer(rveDevice.Device(), vertexBuffer, nullptr);
		vkFreeMemory(rveDevice.Device(), vertexBufferMemory, nullptr);
	}

	void RveModel::Bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void RveModel::Draw(VkCommandBuffer commandBuffer) {
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}

	void RveModel::CreateVertexBuffers(std::vector<Vertex> &vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "(rve_model.cpp) Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		rveDevice.CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory);
		void *data;
		vkMapMemory(rveDevice.Device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(rveDevice.Device(), vertexBufferMemory);
	}

	std::vector<VkVertexInputBindingDescription> RveModel::Vertex::GetBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		bindingDescriptions[0].stride = sizeof(Vertex);
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> RveModel::Vertex::GetAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		return attributeDescriptions;
	}
} // namespace rve
