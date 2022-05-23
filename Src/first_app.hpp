#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_renderer.hpp"
#include "lve_descriptors.hpp"
#include "lve_texture_storage.hpp"

#include <memory>
#include <vector>

namespace lve {

	class FirstApp {

	public:
		static constexpr int WIDTH = 1920;
		static constexpr int HEIGHT = 1080;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		void operator=(const FirstApp&) = delete;

		void run();
	private:
		void loadGameObjects();
		
		LveWindow lveWindow{ WIDTH, HEIGHT, "Vulkan V" };
		LveDevice lveDevice{ lveWindow };
		LveTextureStorage lveTextureStorage{ lveDevice };
		LveRenderer lveRenderer{ lveWindow, lveDevice };

		// note: order of declarations matters
		std::unique_ptr<LveDescriptorPool> globalPool{};
		LveGameObject::Map gameObjects;
	};
}