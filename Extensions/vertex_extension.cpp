#include "vertex_extension.hpp"

namespace vertex_extension {
	
	void fillVertex(
		std::vector<lve::LveModel::Vertex>& verticies,
		lve::LveModel::Vertex& startVector,
		uint32_t depth
	) {
		verticies.push_back(startVector);
		lve::LveModel::Vertex vertex2 = { {startVector.position.x - 0.05f, startVector.position.y + 0.1f}};
		verticies.push_back(vertex2);
		lve::LveModel::Vertex vertex3 = { {startVector.position.x + 0.05f, startVector.position.y + 0.1f} };
		verticies.push_back(vertex3);

		uint32_t nextLevel = depth - 1;
		if (nextLevel <= 0)
			return;

		fillVertex(verticies, vertex2, nextLevel);
		fillVertex(verticies, vertex3, nextLevel);
	}
}