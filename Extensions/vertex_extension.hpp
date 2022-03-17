#include "../lve_model.hpp"

#include <vector>

namespace vertex_extension {

	void fillVertex(
		std::vector<lve::LveModel::Vertex>& verticies,
		lve::LveModel::Vertex& startVector,
		uint32_t depth,
		float_t stepBetweenUpVertex
	);
}