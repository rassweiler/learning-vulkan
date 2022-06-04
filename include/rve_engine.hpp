#pragma once

#include "rve_pipeline.hpp"
#include "rve_window.hpp"
#include "rve_vulkan_device.hpp"
#include "rve_game_object.hpp"
#include "rve_renderer.hpp"
#include "rve_render_system.hpp"

#include <memory>
#include <vector>

namespace rve {
	class RveEngine {
	public:
		RveEngine();
		~RveEngine();
		RveEngine(const RveEngine &) = delete;
		RveEngine &operator=(const RveEngine &) = delete;

		void Run();

		static constexpr int windowWidth = 600;
		static constexpr int windowHeight = 600;
	
	private:
		void LoadGameObjects();
		std::unique_ptr<RveModel> Create3DTestModel(RveVulkanDevice& device, glm::vec3 offset);

		RveWindow rveWindow{windowWidth, windowHeight, "Vulkan Test"};
		RveVulkanDevice rveVulkanDevice{rveWindow};
		RveRenderer rveRenderer{rveWindow, rveVulkanDevice};
		std::vector<RveGameObject> rveGameObjects;
	};
} // namespace rve
