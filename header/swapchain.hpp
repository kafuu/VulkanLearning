#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {
	class Swapchain final {
	public:
		vk::SwapchainKHR swapchain;

		Swapchain(int w, int h);
		~Swapchain();

		struct SwapchainInfo {
			vk::Extent2D imageExtent;//ͼ���С
			uint32_t imageCount;//ͼ������
			vk::SurfaceFormatKHR imageFormat;//��ɫ����
			vk::SurfaceTransformFlagsKHR transform;//��ͼ����еĲ���
			vk::PresentModeKHR present;

		};

		SwapchainInfo info;

		void queryInfo(int w, int h);
	};


}