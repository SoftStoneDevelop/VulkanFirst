#pragma once

#include "lve_camera.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"
#include "lve_frame_info.hpp"
#include "lve_texture_storage.hpp"
#include "lve_descriptors.hpp"

#include <memory>
#include <vector>

namespace lve {

	class SimpleRenderSystem {

	public:

		SimpleRenderSystem(
			LveDevice& device,
			LveTextureStorage& lveTextureStorage,
			VkRenderPass renderPass,
			LveDescriptorSetLayout& globalSetLayout,
			LveDescriptorSetLayout& textureSetLayout,
			LveDescriptorPool& pool,
			std::vector<VkDescriptorSet>& descriptorSets
		);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		void operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		LveDevice& lveDevice;
		LveTextureStorage& lveTextureStorage;
		LveDescriptorPool& texturePool;

		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
		LveDescriptorSetLayout& textureSetLayout;
		std::vector<VkDescriptorSet>& descriptorSets;
	};
}