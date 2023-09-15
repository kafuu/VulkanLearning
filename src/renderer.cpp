#include "renderer.hpp"
#include "context.hpp"


namespace toy2d {

	Renderer::Renderer(int maxFlightCount):maxFlightCount(maxFlightCount),currentFrame(0){
		InitCommandPool();
		allocateCommandBuffer();
		createSemaphore();
		createFence();
	}

	Renderer::~Renderer() {
		Context::GetInstance().device.freeCommandBuffers(commandPool, commandBuffers[currentFrame]);
		Context::GetInstance().device.destroyCommandPool(commandPool);
		for (auto& semaphore : imageAvaliables) {

			Context::GetInstance().device.destroySemaphore(semaphore);
		}
		for (auto& semaphore : imageDrawFinishs) {

			Context::GetInstance().device.destroySemaphore(semaphore);
		}
		for (auto& fence : commandAvailableFences) {
			Context::GetInstance().device.destroyFence(fence);
		}
	}

	void Renderer::InitCommandPool() {
		vk::CommandPoolCreateInfo createInfo;
		createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);//
		commandPool = Context::GetInstance().device.createCommandPool(createInfo);
	}

	void Renderer::allocateCommandBuffer() {
		vk::CommandBufferAllocateInfo allocateInfo;
		allocateInfo.setCommandPool(commandPool)
			.setCommandBufferCount(2)
			.setLevel(vk::CommandBufferLevel::ePrimary);//一个buffer可以控制别的commandbuffer，控制别人的就是primary
		
		commandBuffers= Context::GetInstance().device.allocateCommandBuffers(allocateInfo);
	}
	
	void Renderer::createFence() {
			commandAvailableFences.resize(maxFlightCount);
			for (auto &fence : commandAvailableFences) {
				vk::FenceCreateInfo createInfo;
				createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
				fence = Context::GetInstance().device.createFence(createInfo);
			}
		}

	void Renderer::createSemaphore() {
		imageAvaliables.resize(maxFlightCount);
		imageDrawFinishs.resize(maxFlightCount);
		for (auto& semaphore : imageAvaliables) {
			vk::SemaphoreCreateInfo createInfo;
			semaphore = Context::GetInstance().device.createSemaphore(createInfo);
		}
		for (auto& semaphore : imageDrawFinishs) {
			vk::SemaphoreCreateInfo createInfo;
			semaphore = Context::GetInstance().device.createSemaphore(createInfo);
		}
	}

	//绘制三角形
	void Renderer::drawTriangle() {
		//同步
		if (Context::GetInstance().device.waitForFences(commandAvailableFences[currentFrame], true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
			std::cout << "wait for fence failed" << std::endl;
		}
		Context::GetInstance().device.resetFences(commandAvailableFences[currentFrame]);


		auto& device = Context::GetInstance().device;
		auto& renderProcess = Context::GetInstance().renderProcess;
		auto& swapchain = Context::GetInstance().swapchain;

		auto result = device.acquireNextImageKHR(Context::GetInstance().swapchain->swapchain,
			std::numeric_limits<uint64_t>::max(), imageAvaliables[currentFrame]);

		if (result.result != vk::Result::eSuccess) {
			std::cout << "acquire next image failed" << std::endl;
		}
		auto imageIndex = result.value;

		commandBuffers[currentFrame].reset();
		
		//开始塞命令
		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		commandBuffers[currentFrame].begin(beginInfo);
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
			commandBuffers[currentFrame].beginRenderPass(renderpassBeginInfo, {});
			commandBuffers[currentFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->pipline);
			{
				//开始绘制 
				commandBuffers[currentFrame].draw(3, 1, 0, 0);
			}
			commandBuffers[currentFrame].endRenderPass();
		}
		commandBuffers[currentFrame].end();

		//已经把commmandbuffer塞好了，开始传gpu(图形队列)
		vk::SubmitInfo submitInfo;
		vk::PipelineStageFlags dstStageMask[] = {
	vk::PipelineStageFlagBits::eBottomOfPipe
		};
		
		submitInfo.setCommandBuffers(commandBuffers[currentFrame])
			.setWaitSemaphores(imageAvaliables[currentFrame])
			.setSignalSemaphores(imageDrawFinishs[currentFrame])
			.setPWaitDstStageMask(dstStageMask);
		Context::GetInstance().graphicsQueue.submit(submitInfo,commandAvailableFences[currentFrame]);//fence用于cpugpu同步，防止gpu还没画完cpu就开始下一个循环了

		//传完了gpu开始画了，画完了就要传给显示器了(显示队列)
		vk::PresentInfoKHR presentInfo;
		presentInfo.setImageIndices(imageIndex)
			.setSwapchains(swapchain->swapchain)
			.setWaitSemaphores(imageDrawFinishs[currentFrame]);
		if (Context::GetInstance().presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
			std::cout << "image present failed" << std::endl;	
		}

		currentFrame = (currentFrame + 1) % maxFlightCount;

		}

	


}