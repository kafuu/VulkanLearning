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
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageViews;
		std::vector<vk::Framebuffer> frameBuffers;

		void getImages();
		void createImageViews();
		void createFrameBuffers(int w, int h);
		void queryInfo(int w, int h);
	};


}