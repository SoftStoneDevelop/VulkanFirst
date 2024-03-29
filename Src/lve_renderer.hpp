#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace lve {

	class LveRenderer {

	public:

		LveRenderer(LveWindow& window, LveDevice& device);
		~LveRenderer();

		LveRenderer(const LveRenderer&) = delete;
		void operator=(const LveRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return lveSwapChain->getRenderPass(); }
		float getAspectRation() const { return lveSwapChain->extentAspectRatio(); };
		bool isFrameInProgress() const { return isFrameStarted; };

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const { 
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		/// <returns>Last frame count or current builded frame, if frame in started</returns>
		int getCurrentFrameCount() const noexcept
		{
			return globalFrameCounter.load();
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		LveWindow& lveWindow;
		LveDevice& lveDevice;

		std::unique_ptr<LveSwapChain> lveSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		std::atomic<int> globalFrameCounter;

		bool isFrameStarted;
	};
}