
#pragma once

#include "vulkan/vulkan.hpp"
#include <iostream>
#include "vertex.hpp"
#include "buffer.hpp"

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

		std::unique_ptr<Buffer> hostVertexBuffer_;//cpuµÄbuffer
		std::unique_ptr<Buffer> deviceVertexBuffer_;//gpuµÄ

		void InitCommandPool();
		void allocateCommandBuffer();
		void drawTriangle();
		void createFence();
		void createSemaphore();
		Renderer(int maxFlightCount = 2);
		void createVertexBuffer();
		void bufferVertexData();
		~Renderer();
	};


}
