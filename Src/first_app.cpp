#pragma once

#define IMGUI_DISABLE_INCLUDE_IMCONFIG_H = true
#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "Systems/simple_render_system.hpp"
#include "Systems/point_light_system.hpp"
#include "lve_buffer.hpp"
#include "Definitions/DefaultSamplersNames.hpp"

//libs
#define GLM_FORCE_RADIANSE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>
#include <numeric>
#include "imgui.hpp"

namespace lve {

	FirstApp::FirstApp() {
		globalPool = LveDescriptorPool::Builder(lveDevice)
			.setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		imGuiPool = LveDescriptorPool::Builder(lveDevice)
			.setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		InitializeImGui(lveWindow, lveDevice, lveRenderer->getSwapChainRenderPass(), imGuiPool->getDescriptorPool(), LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		loadTextures();
		loadGameObjects();
	}

	FirstApp::~FirstApp() 
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void FirstApp::run() {

		std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<LveBuffer>(
				lveDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build()
			;

		std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			LveDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}
		
		SimpleRenderSystem simpleRenderSystem{
			lveDevice,
			lveTextureStorage,
			lveRenderer->getSwapChainRenderPass(),
			*globalSetLayout
		};

		PointLightSystem pointLightSystem{
			lveDevice,
			lveRenderer->getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout()
		};
        LveCamera camera{};

        auto viewerObject = LveGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		bool c = true;
		while (!lveWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            
            float aspect = lveRenderer->getAspectRation();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			if (auto commandBuffer = lveRenderer->beginFrame())
			{
				int frameIndex = lveRenderer->getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				//update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//render
				lveRenderer->beginSwapChainRenderPass(commandBuffer);

				//order here matters
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);

				ImGuiNewFrame();
				auto tData = lveTextureStorage.getTextureData("statue");
				ImGui::SetNextWindowSizeConstraints(
					{ },
					{ (float)tData.texWidth, (float)tData.texHeight }
				);
				ImGui::Begin("Hello button");
				auto info = lveTextureStorage.getDescriptorSet("statue", defaultSamplerName);
				ImGui::Image(info, { (float)tData.texWidth, (float)tData.texHeight });
				ImGui::End();
				
				ImGuiRender(commandBuffer);

				lveRenderer->endSwapChainRenderPass(commandBuffer);
				lveRenderer->endFrame();
			}
		}

		vkDeviceWaitIdle(lveDevice.device());
	} 

	void FirstApp::loadGameObjects() {
		std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "Models/flat_vase.obj");
        auto flatVase = LveGameObject::createGameObject();
		flatVase.model = lveModel;
		flatVase.transform.translation = { -.5f, .5f, 0.f };
		flatVase.transform.scale = { 3.f, 1.5f, 3.f };
		flatVase.model->setTextureName("statue2");
        gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		lveModel = LveModel::createModelFromFile(lveDevice, "Models/smooth_vase.obj");
		auto smoothVase = LveGameObject::createGameObject();
		smoothVase.model = lveModel;
		smoothVase.transform.translation = { .5f, .5f, 0.f };
		smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
		smoothVase.model->setTextureName("statue3");
		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		lveModel = LveModel::createModelFromFile(lveDevice, "Models/quad.obj");
		auto floor = LveGameObject::createGameObject();
		floor.model = lveModel;
		floor.transform.translation = { 0.f, .5f, 0.f };
		floor.transform.scale = { 3.f, 1.f, 3.f };
		floor.model->setTextureName("statue");
		gameObjects.emplace(floor.getId(), std::move(floor));

		auto pointLight = LveGameObject::makePointLight(0.02f);
		gameObjects.emplace(pointLight.getId(), std::move(pointLight));

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}  //
		};

		for (int i = 0; i < lightColors.size(); i++)
		{
			auto pointLight = LveGameObject::makePointLight(0.7f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f }
			);
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));

			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
	}

	void FirstApp::loadTextures()
	{
		VkSamplerCreateInfo sampler{};
		sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler.magFilter = VK_FILTER_LINEAR;
		sampler.minFilter = VK_FILTER_LINEAR;
		sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler.anisotropyEnable = VK_TRUE;
		sampler.maxAnisotropy = lveDevice.properties.limits.maxSamplerAnisotropy;
		sampler.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler.unnormalizedCoordinates = VK_FALSE;
		sampler.compareEnable = VK_FALSE;
		sampler.compareOp = VK_COMPARE_OP_ALWAYS;

		lveTextureStorage.loadTexture("Textures/statue.jpg", "statue", sampler);
		lveTextureStorage.loadTexture("Textures/statue2.jpg", "statue2", sampler);
		lveTextureStorage.loadTexture("Textures/statue3.jpg", "statue3", sampler);
	}
}