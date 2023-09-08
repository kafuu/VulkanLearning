#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {
	class RenderProcess final {
	public:
		vk::Pipeline pipline;
		vk::PipelineLayout layout;
		vk::RenderPass renderPass; 

		~RenderProcess();
		void InitLayout();
		void InitrenderPass();
		void InitPipline(int width, int height);
		
	};

}