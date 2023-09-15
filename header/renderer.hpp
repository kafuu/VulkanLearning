#pragma once

#include "vulkan/vulkan.hpp"
#include <iostream>

namespace toy2d {
	class Renderer final {
	public:
		vk::CommandPool commandPool;
		std::vector<vk::CommandBuffer> commandBuffers;
		std::vector<vk::Fence> commandAvailableFences;
		std::vector<vk::Semaphore> imageAvaliables;
		std::vector<vk::Semaphore> imageDrawFinishs;
		int maxFlightCount;
		int currentFrame;

		void InitCommandPool();
		void allocateCommandBuffer();
		void drawTriangle();
		void createFence();
		void createSemaphore();
		Renderer(int maxFlightCount = 2);
		~Renderer();
	};


}