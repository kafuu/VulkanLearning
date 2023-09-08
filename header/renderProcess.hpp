#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {
	class RenderProcess final {
	public:
		vk::Pipeline pipline;
		void InitPipline(int width, int height);
		void DestoryPipline();
		
	};

}