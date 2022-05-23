#pragma once

#include <string>
#include <vector>

#include "rve_window.hpp"

namespace rve {
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR vkCapabilities;
		std::vector<VkSurfaceFormatKHR> vkFormats;
		std::vector<VkPresentModeKHR> vkPresentModes;
	};

	struct QueueFamilyIndices {
		uint32_t graphicsFamily;
		uint32_t presentFamily;
		bool graphicsFamilyHasValue = false;
		bool presentFamilyHasValue = false;
		bool isComplete() {
			return graphicsFamilyHasValue && presentFamilyHasValue;
		}
	};

	class RveVulkanDevice {
	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();

		bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
		std::vector<const char *> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
		void HasGflwRequiredInstanceExtensions();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice);

		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		RveWindow &window;
		VkCommandPool commandPool;

		VkDevice device_;
		VkSurfaceKHR surface_;
		VkQueue graphicsQueue_;
		VkQueue presentQueue_;

		const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
		const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	public:
		#ifdef NDEBUG
			const bool enableValidationLayers = false;
		#else
			const bool enableValidationLayers = true;
		#endif

		RveVulkanDevice(RveWindow &window);
		~RveVulkanDevice();
		RveVulkanDevice(const RveVulkanDevice &) = delete;
		RveVulkanDevice &operator=(const RveVulkanDevice &) = delete;

		VkCommandPool GetCommandPool() { return commandPool; }
		VkDevice Device() { return device_; }
		VkSurfaceKHR Surface() { return surface_; }
		VkQueue GraphicsQueue() { return graphicsQueue_; }
		VkQueue PresentQueue() { return presentQueue_; }

		SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(physicalDevice); }
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(physicalDevice); }
		VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		// Buffer Helper Functions
		void CreateBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer &buffer,
			VkDeviceMemory &bufferMemory);
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

		void CreateImageWithInfo(
			const VkImageCreateInfo &imageInfo,
			VkMemoryPropertyFlags properties,
			VkImage &image,
			VkDeviceMemory &imageMemory);

		VkPhysicalDeviceProperties properties;
	};
} // namespace rve
