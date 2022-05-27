#pragma once

#include "rve_model.hpp"

#include <memory>

namespace rve {
	struct Transform2dComponent {
		glm::vec2 translation{};
		glm::vec2 scale{1.0f, 1.0f};
		float rotation;
		glm::mat2 mat2() { 
			const float sin = glm::sin(rotation);
			const float cos = glm::cos(rotation);
			glm::mat2 rotationMatrix = {{cos, sin}, {-sin, cos}};
			glm::mat2 scaleMatrix{{scale.x, 0.0f}, {0.0f, scale.y}};
			return rotationMatrix * scaleMatrix;
		}
	};

	class RveGameObject {
		public:
		using id_t = unsigned int;

		RveGameObject(const RveGameObject &) = delete;
		RveGameObject &operator=(const RveGameObject &) = delete;
		RveGameObject(RveGameObject &&) = default;
		RveGameObject &operator=(RveGameObject &&) = default;

		static RveGameObject CreateGameObject() {
			static id_t currentId = 0;
			return RveGameObject{currentId++};
		};
		const id_t GetId() { return id; }

		std::shared_ptr<RveModel> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d{};

		private:
		RveGameObject(id_t objectId) : id{objectId} {};
		id_t id;
	};
} // namespace rve
