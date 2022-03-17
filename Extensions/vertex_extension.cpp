#include "vertex_extension.hpp"

namespace vertex_extension {
	
	void fillVertex(
		std::vector<lve::LveModel::Vertex>& verticies,
		lve::LveModel::Vertex& startVector,
		uint32_t depth,
		float_t stepBetweenUpVertex
	) {
		lve::LveModel::Vertex startLevel;
		lve::LveModel::Vertex currentUpTriangleVertex;
		startLevel = startVector;
		currentUpTriangleVertex = startVector;

		float_t stepDownVertex = stepBetweenUpVertex / 2;

		verticies.push_back(startLevel);
		for (uint32_t i = 1; i<= depth; i++)
		{
			for (uint32_t j = 1; j <= i; j++)
			{
				lve::LveModel::Vertex vertex2 = { {currentUpTriangleVertex.position.x - stepDownVertex, currentUpTriangleVertex.position.y + stepBetweenUpVertex} };
				verticies.push_back(vertex2);
				lve::LveModel::Vertex vertex3 = { {currentUpTriangleVertex.position.x + stepDownVertex, currentUpTriangleVertex.position.y + stepBetweenUpVertex} };
				verticies.push_back(vertex3);

				currentUpTriangleVertex = { {currentUpTriangleVertex.position.x + stepBetweenUpVertex, currentUpTriangleVertex.position.y} };

				if(j != i)
					verticies.push_back(currentUpTriangleVertex);
			}

			startLevel = { {startLevel.position.x - stepDownVertex, startLevel.position.y + stepBetweenUpVertex} };
			currentUpTriangleVertex = startLevel;
			verticies.push_back(startLevel);
		}
	}
}