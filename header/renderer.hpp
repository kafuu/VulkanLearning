#pragma once

#include "vulkan/vulkan.hpp"
#include <iostream>

namespace toy2d {
	class Renderer final {
	public:
		vk::CommandPool commandPool;
		vk::CommandBuffer commandBuffer;
		vk::Fence commandAvailableFence;
		vk::Semaphore imageAvaliable;
		vk::Semaphore imageDrawFinish;

		void InitCommandPool();
		void allocateCommandBuffer();
		void render();
		void createFence();
		void createSemaphore();
		Renderer();
		~Renderer();
	};


}