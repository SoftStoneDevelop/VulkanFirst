#pragma once

#include "lve_buffer.hpp"
#include "lve_device.hpp"
#include "Definitions/DefaultSamplersNames.hpp"
#include "lve_swap_chain.hpp"

//libs
#define GLM_FORCE_RADIANSE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <memory>
#include <vector>

namespace lve {
	class LveModel
	{
	public:

		struct  Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};
			glm::vec2 textCoord{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return
					position == other.position &&
					color == other.color &&
					normal == other.normal &&
					uv == other.uv &&
					textCoord == other.textCoord;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		LveModel(LveDevice& lveDevice, const LveModel::Builder& builder);

		~LveModel();

		LveModel(const LveModel&) = delete;
		void operator=(const LveModel&) = delete;

		static std::unique_ptr<LveModel> createModelFromFile(LveDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		void setTextureName(std::string&& textureName);
		std::string& getTextureName();

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		LveDevice& lveDevice;

		std::unique_ptr<LveBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<LveBuffer> indexBuffer;
		uint32_t indexCount;

		std::string textureName;

		std::string samplerName = defaultSamplerName;
	};
}