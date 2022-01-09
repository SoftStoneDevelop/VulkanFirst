#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"

namespace lve {

	class FirstApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGTH = 600;

		void run();
	private:
		LveWindow lveWindow{ WIDTH, HEIGTH, "Vulkan Hellp!" };
		LveDevice lveDevice{ lveWindow };
		LvePipeline lvePipeLine{ 
			lveDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGTH)
		};
	};
}