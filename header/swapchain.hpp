#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {
	class Swapchain final {
	public:
		vk::SwapchainKHR swapchain;

		Swapchain(int w, int h);
		~Swapchain();

		struct SwapchainInfo {
			vk::Extent2D imageExtent;//图像大小
			uint32_t imageCount;//图像数量
			vk::SurfaceFormatKHR imageFormat;//颜色属性
			vk::SurfaceTransformFlagsKHR transform;//对图像进行的操作
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