#include "vertex_extension.hpp"

namespace vertex_extension {
	
	void createAndFillVertex(std::vector<lve::LveModel::Vertex>*& verticies) {
		verticies = new std::vector<lve::LveModel::Vertex>{
			{{0.0f, -0.5f}},
			{{0.5f, 0.5f}},
			{{-0.5f, 0.5f}},

			{{0.5f, 0.5f}},
			{{1.0f, 1.5f}},
			{{0.0f, 1.5f}},

			{{-0.5f, 0.5f}},
			{{0.0f, 1.5f}},
			{{-1.0f, 1.5f}},
		};
	}
}