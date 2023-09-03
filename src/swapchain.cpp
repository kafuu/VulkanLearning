#include "swapchain.hpp"
#include "context.hpp"

namespace toy2d {
	Swapchain::Swapchain(int w, int h){
		queryInfo(w, h);
		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(true)//裁切
			.setImageArrayLayers(1)//vulkan以数组存储图片，可以有多层（layer）数组
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)//图像的用途是颜色附件，也就是绘制出来，除此还有深度用途等等
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)//窗口的颜色和gpu的图片的融混方式：不容混
			.setSurface(Context::GetInstance().surface)//用作单例的原因是？？
			.setImageColorSpace(info.imageFormat.colorSpace)
			.setImageFormat(info.imageFormat.format)
			.setMinImageCount(info.imageCount)
			.setImageExtent(info.imageExtent)
			.setPresentMode(info.present);

		auto queueIndicies = Context::GetInstance().queueFamilyIndices;
		if (queueIndicies.graphicsQuene.value() == queueIndicies.presentQuene.value()) {
			createInfo.setQueueFamilyIndices(queueIndicies.graphicsQuene.value())
				.setImageSharingMode(vk::SharingMode::eExclusive);

		}
		else {
			std::array indices = {queueIndicies.graphicsQuene.value(),queueIndicies.presentQuene.value()};
			createInfo.setQueueFamilyIndices(indices)
				.setImageSharingMode(vk::SharingMode::eConcurrent);//两个命令队列并行
		}

		swapchain = Context::GetInstance().device.createSwapchainKHR(createInfo);//在device上创建swapchain
		  
	}

	Swapchain::~Swapchain() {
		Context::GetInstance().device.destroySwapchainKHR(swapchain);
	}

	void Swapchain::queryInfo(int w, int h) {
		auto& phyDevice = Context::GetInstance().phyDevice;
		auto& surface = Context::GetInstance().surface;

		auto formats = phyDevice.getSurfaceFormatsKHR(surface);
		info.imageFormat = formats[0];
		for (const auto& format : formats) {
				if (format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear &&
					format.format == vk::Format::eR8G8B8A8Srgb) {
					info.imageFormat = format;
					break;
				}
		}

		auto capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
		info.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);

		info.imageExtent.width = std::clamp<uint32_t>(w, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		info.imageExtent.height = std::clamp<uint32_t>(h, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		
		info.transform = capabilities.currentTransform;

		auto presents = phyDevice.getSurfacePresentModesKHR(surface);
		info.present = vk::PresentModeKHR::eFifo;
		for (const auto& present : presents) {
			if (present == vk::PresentModeKHR::eMailbox) {
				info.present = present;
				break;
			}
		}

	}

}