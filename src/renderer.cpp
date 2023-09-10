#include "renderer.hpp"
#include "context.hpp"


namespace toy2d {

	Renderer::Renderer() {
		InitCommandPool();
		allocateCommandBuffer();
		createSemaphore();
		createFence();
	}

	Renderer::~Renderer() {
		Context::GetInstance().device.freeCommandBuffers(commandPool, commandBuffer);
		Context::GetInstance().device.destroyCommandPool(commandPool);
		Context::GetInstance().device.destroySemaphore(imageAvaliable);
		Context::GetInstance().device.destroySemaphore(imageDrawFinish);
		Context::GetInstance().device.destroyFence(commandAvailableFence);
	}

	void Renderer::InitCommandPool() {
		vk::CommandPoolCreateInfo createInfo;
		createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);//
		commandPool = Context::GetInstance().device.createCommandPool(createInfo);
	}

	void Renderer::allocateCommandBuffer() {
		vk::CommandBufferAllocateInfo allocateInfo;
		allocateInfo.setCommandPool(commandPool)
			.setCommandBufferCount(1)
			.setLevel(vk::CommandBufferLevel::ePrimary);//һ��buffer���Կ��Ʊ��commandbuffer�����Ʊ��˵ľ���primary
			
		commandBuffer = Context::GetInstance().device.allocateCommandBuffers(allocateInfo)[0];
	}


	//����������
	void Renderer::render() {
		auto& device = Context::GetInstance().device;
		auto& renderProcess = Context::GetInstance().renderProcess;
		auto& swapchain = Context::GetInstance().swapchain;

		auto result = device.acquireNextImageKHR(Context::GetInstance().swapchain->swapchain,
			std::numeric_limits<uint64_t>::max(), imageAvaliable);

		if (result.result != vk::Result::eSuccess) {
			std::cout << "acquire next image failed" << std::endl;
		}
		auto imageIndex = result.value;

		commandBuffer.reset();
		
		//��ʼ������
		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		commandBuffer.begin(beginInfo);
		{
			//1.��pipline
			
			//2.��ʼrenderpass��Ⱦ
			vk::RenderPassBeginInfo renderpassBeginInfo;
			vk::Rect2D area;
			vk::ClearValue clearvalue;
			area.setOffset({0,0})
				.setExtent(swapchain->info.imageExtent);//��Ļ��С
			clearvalue.color = vk::ClearColorValue(std::array<float,4> {0.1f, 0.1f, 0.1f,1.0f});
			renderpassBeginInfo.setRenderPass(renderProcess->renderPass)
				.setRenderArea(area)
				.setFramebuffer(swapchain->frameBuffers[imageIndex])//�ҵ�imageIndex��Ӧ��framebuffer
				.setClearValues(clearvalue);//clear��ɫ
			commandBuffer.beginRenderPass(renderpassBeginInfo, {});
			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->pipline);
			{
				//��ʼ����
				commandBuffer.draw(3, 1, 0, 0);
			}
			commandBuffer.endRenderPass();
		}
		commandBuffer.end();

		//�Ѿ���commmandbuffer�����ˣ���ʼ��gpu(ͼ�ζ���)
		vk::SubmitInfo submitInfo;
		vk::PipelineStageFlags dstStageMask[] = {
	vk::PipelineStageFlagBits::eBottomOfPipe
		};
		
		submitInfo.setCommandBuffers(commandBuffer)
			.setWaitSemaphores(imageAvaliable)
			.setSignalSemaphores(imageDrawFinish)
			.setPWaitDstStageMask(dstStageMask);
		Context::GetInstance().graphicsQueue.submit(submitInfo,commandAvailableFence);//fence����cpugpuͬ������ֹgpu��û����cpu�Ϳ�ʼ��һ��ѭ����

		//������gpu��ʼ���ˣ������˾�Ҫ������ʾ����(��ʾ����)
		vk::PresentInfoKHR presentInfo;
		presentInfo.setImageIndices(imageIndex)
			.setSwapchains(swapchain->swapchain)
			.setWaitSemaphores(imageDrawFinish);
		if (Context::GetInstance().presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
			std::cout << "image present failed" << std::endl;	
		}

		//ͬ��
		if (Context::GetInstance().device.waitForFences(commandAvailableFence, true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
			std::cout << "wait for fence failed" << std::endl;
		}
		Context::GetInstance().device.resetFences(commandAvailableFence);
	}

	void Renderer::createFence() {
		vk::FenceCreateInfo createInfo;
		commandAvailableFence = Context::GetInstance().device.createFence(createInfo);
	}

	void Renderer::createSemaphore() {
		vk::SemaphoreCreateInfo createInfo;
		imageAvaliable = Context::GetInstance().device.createSemaphore(createInfo);
		imageDrawFinish = Context::GetInstance().device.createSemaphore(createInfo);
	}
}