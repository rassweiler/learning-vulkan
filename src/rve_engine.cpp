#include "../include/rve_engine.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <array>

namespace rve {
	RveEngine::RveEngine() {
		LoadGameObjects();
	}

	RveEngine::~RveEngine() {

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

	void RveEngine::Run() {
		RveRenderSystem renderSystem{rveVulkanDevice, rveRenderer.GetSwapChainRenderPass()};

		while(!rveWindow.ShouldClose()) {
			glfwPollEvents();
			
			if(auto commandBuffer = rveRenderer.BeginFrame()) {
				rveRenderer.BeginSwapChainRenderPass(commandBuffer);
				renderSystem.RenderGameObjects(commandBuffer, rveGameObjects);
				rveRenderer.EndSwapChainRenderPass(commandBuffer);
				rveRenderer.EndFrame();
			}
		}
		vkDeviceWaitIdle(rveVulkanDevice.Device());
	}
} // namespace rve
