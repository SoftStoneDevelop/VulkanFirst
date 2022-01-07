#pragma once

#include "lve_window.hpp"

namespace lve {

	class FirstApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGTH = 600;

		void run();
	private:
		LveWindow lveWindow{ WIDTH, HEIGTH, "Vulkan Hellp!" };
	};
}