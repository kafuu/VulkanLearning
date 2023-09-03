#include "swapchain.hpp"
#include "context.hpp"

namespace toy2d {
	Swapchain::Swapchain(int w, int h){
		queryInfo(w, h);
		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(true)//����
			.setImageArrayLayers(1)//vulkan������洢ͼƬ�������ж�㣨layer������
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)//ͼ�����;����ɫ������Ҳ���ǻ��Ƴ��������˻��������;�ȵ�
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)//���ڵ���ɫ��gpu��ͼƬ���ڻ췽ʽ�����ݻ�
			.setSurface(Context::GetInstance().surface)//����������ԭ���ǣ���
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
				.setImageSharingMode(vk::SharingMode::eConcurrent);//����������в���
		}

		swapchain = Context::GetInstance().device.createSwapchainKHR(createInfo);//��device�ϴ���swapchain
		  
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