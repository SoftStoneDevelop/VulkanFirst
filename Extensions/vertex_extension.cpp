#include "vertex_extension.hpp"

namespace vertex_extension {
	
	void fillVertex(
		std::vector<lve::LveModel::Vertex>& verticies,
		lve::LveModel::Vertex& startVector,
		uint32_t depth
	) {
		lve::LveModel::Vertex startLevel;
		lve::LveModel::Vertex currentUpTriangleVertex;
		startLevel = startVector;
		currentUpTriangleVertex = startVector;

		verticies.push_back(startLevel);
		for (uint32_t i = 1; i<= depth; i++)
		{
			for (uint32_t j = 1; j <= i; j++)
			{
				lve::LveModel::Vertex vertex2 = { {currentUpTriangleVertex.position.x - 0.005f, currentUpTriangleVertex.position.y + 0.01f} };
				verticies.push_back(vertex2);
				lve::LveModel::Vertex vertex3 = { {currentUpTriangleVertex.position.x + 0.005f, currentUpTriangleVertex.position.y + 0.01f} };
				verticies.push_back(vertex3);

				currentUpTriangleVertex = { {currentUpTriangleVertex.position.x + 0.01f, currentUpTriangleVertex.position.y} };

				if(j != i)
					verticies.push_back(currentUpTriangleVertex);
			}

			startLevel = { {startLevel.position.x - 0.005f, startLevel.position.y + 0.01f} };
			currentUpTriangleVertex = startLevel;
			verticies.push_back(startLevel);
		}
	}
}