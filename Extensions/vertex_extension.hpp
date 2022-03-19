#include "../lve_model.hpp"

#include <vector>
#include <glm/glm.hpp>

namespace vertex_extension {

	void fillVertex(
		std::vector<lve::LveModel::Vertex>& verticies,
		lve::LveModel::Vertex& startVector,
		uint32_t depth,
		float_t stepBetweenUpVertex,
		glm::vec3 colorVertex1,
		glm::vec3 colorVertex2,
		glm::vec3 colorVertex3
	);
}