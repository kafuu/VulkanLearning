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
			.setLevel(vk::CommandBufferLevel::ePrimary);//一个buffer可以控制别的commandbuffer，控制别人的就是primary
			
		commandBuffer = Context::GetInstance().device.allocateCommandBuffers(allocateInfo)[0];
	}


	//绘制三角形
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
		
		//开始塞命令
		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		commandBuffer.begin(beginInfo);
		{
			//1.绑定pipline
			
			//2.开始renderpass渲染
			vk::RenderPassBeginInfo renderpassBeginInfo;
			vk::Rect2D area;
			vk::ClearValue clearvalue;
			area.setOffset({0,0})
				.setExtent(swapchain->info.imageExtent);//屏幕大小
			clearvalue.color = vk::ClearColorValue(std::array<float,4> {0.1f, 0.1f, 0.1f,1.0f});
			renderpassBeginInfo.setRenderPass(renderProcess->renderPass)
				.setRenderArea(area)
				.setFramebuffer(swapchain->frameBuffers[imageIndex])//找到imageIndex对应的framebuffer
				.setClearValues(clearvalue);//clear颜色
			commandBuffer.beginRenderPass(renderpassBeginInfo, {});
			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->pipline);
			{
				//开始绘制
				commandBuffer.draw(3, 1, 0, 0);
			}
			commandBuffer.endRenderPass();
		}
		commandBuffer.end();

		//已经把commmandbuffer塞好了，开始传gpu(图形队列)
		vk::SubmitInfo submitInfo;
		vk::PipelineStageFlags dstStageMask[] = {
	vk::PipelineStageFlagBits::eBottomOfPipe
		};
		
		submitInfo.setCommandBuffers(commandBuffer)
			.setWaitSemaphores(imageAvaliable)
			.setSignalSemaphores(imageDrawFinish)
			.setPWaitDstStageMask(dstStageMask);
		Context::GetInstance().graphicsQueue.submit(submitInfo,commandAvailableFence);//fence用于cpugpu同步，防止gpu还没画完cpu就开始下一个循环了

		//传完了gpu开始画了，画完了就要传给显示器了(显示队列)
		vk::PresentInfoKHR presentInfo;
		presentInfo.setImageIndices(imageIndex)
			.setSwapchains(swapchain->swapchain)
			.setWaitSemaphores(imageDrawFinish);
		if (Context::GetInstance().presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
			std::cout << "image present failed" << std::endl;	
		}

		//同步
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