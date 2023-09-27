#include "renderer.hpp"
#include "context.hpp"


namespace toy2d {

	std::array<Vertex, 3> verticies = {
			Vertex{-0.5, 0.5},
			Vertex{0.5, 0.5},
			Vertex{0, -1.0}
	};

	Renderer::Renderer(int maxFlightCount) :maxFlightCount(maxFlightCount), currentFrame(0) {
		InitCommandPool();
		allocateCommandBuffer();
		createSemaphore();
		createFence();
		createVertexBuffer();//ֻ�Ǵ�����û����
		bufferVertexData();//������vertices
	}

	Renderer::~Renderer() {
		hostVertexBuffer_.reset();
		deviceVertexBuffer_.reset();
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
			.setLevel(vk::CommandBufferLevel::ePrimary);//һ��buffer���Կ��Ʊ��commandbuffer�����Ʊ��˵ľ���primary

		commandBuffers = Context::GetInstance().device.allocateCommandBuffers(allocateInfo);
	}

	void Renderer::createFence() {
		commandAvailableFences.resize(maxFlightCount);
		for (auto& fence : commandAvailableFences) {
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

	//����������
	void Renderer::drawTriangle() {
		//ͬ��
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

		//��ʼ������
		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		commandBuffers[currentFrame].begin(beginInfo);
		{
			//1.��pipline

			//2.��ʼrenderpass��Ⱦ
			vk::RenderPassBeginInfo renderpassBeginInfo;
			vk::Rect2D area;
			vk::ClearValue clearvalue;
			area.setOffset({ 0,0 })
				.setExtent(swapchain->info.imageExtent);//��Ļ��С
			clearvalue.color = vk::ClearColorValue(std::array<float, 4> {0.1f, 0.1f, 0.1f, 1.0f});
			renderpassBeginInfo.setRenderPass(renderProcess->renderPass)
				.setRenderArea(area)
				.setFramebuffer(swapchain->frameBuffers[imageIndex])//�ҵ�imageIndex��Ӧ��framebuffer
				.setClearValues(clearvalue);//clear��ɫ
			commandBuffers[currentFrame].beginRenderPass(renderpassBeginInfo, {});
			commandBuffers[currentFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->pipline);
			{
				vk::DeviceSize offset = 0;
				commandBuffers[currentFrame].bindVertexBuffers(0, deviceVertexBuffer_->buffer, offset);
				//��ʼ���� 
				commandBuffers[currentFrame].draw(3, 1, 0, 0);
			}
			commandBuffers[currentFrame].endRenderPass();
		}
		commandBuffers[currentFrame].end();

		//�Ѿ���commmandbuffer�����ˣ���ʼ��gpu(ͼ�ζ���)
		vk::SubmitInfo submitInfo;
		vk::PipelineStageFlags dstStageMask[] = {
	vk::PipelineStageFlagBits::eBottomOfPipe
		};

		submitInfo.setCommandBuffers(commandBuffers[currentFrame])
			.setWaitSemaphores(imageAvaliables[currentFrame])
			.setSignalSemaphores(imageDrawFinishs[currentFrame])
			.setPWaitDstStageMask(dstStageMask);
		Context::GetInstance().graphicsQueue.submit(submitInfo, commandAvailableFences[currentFrame]);//fence����cpugpuͬ������ֹgpu��û����cpu�Ϳ�ʼ��һ��ѭ����

		//������gpu��ʼ���ˣ������˾�Ҫ������ʾ����(��ʾ����)
		vk::PresentInfoKHR presentInfo;
		presentInfo.setImageIndices(imageIndex)
			.setSwapchains(swapchain->swapchain)
			.setWaitSemaphores(imageDrawFinishs[currentFrame]);
		if (Context::GetInstance().presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
			std::cout << "image present failed" << std::endl;
		}

		currentFrame = (currentFrame + 1) % maxFlightCount;

	}
	void Renderer::createVertexBuffer() {
		hostVertexBuffer_.reset(new Buffer(sizeof(verticies),
			vk::BufferUsageFlagBits::eTransferSrc,//������㣬��cpu����gpu
			vk::MemoryPropertyFlagBits::eHostVisible
			| vk::MemoryPropertyFlagBits::eHostCoherent));//cpugpu�����ڴ�
		deviceVertexBuffer_.reset(new Buffer(sizeof(verticies),
			vk::BufferUsageFlagBits::eVertexBuffer
			| vk::BufferUsageFlagBits::eTransferDst,//�����յ�
			vk::MemoryPropertyFlagBits::eHostVisible
			| vk::MemoryPropertyFlagBits::eDeviceLocal));//gpu����

	}
	void Renderer::bufferVertexData() {
		//���ݽ�cpu
		void* ptr = Context::GetInstance().device.mapMemory(hostVertexBuffer_->memory, 0, sizeof(verticies));
		memcpy(ptr, verticies.data(), sizeof(verticies));
		Context::GetInstance().device.unmapMemory(hostVertexBuffer_->memory);

		//��cpu���ݸ�gpu
		vk::CommandBuffer commandbuffer;
		vk::CommandBufferAllocateInfo allocateInfo;
		allocateInfo.setCommandPool(commandPool)
			.setCommandBufferCount(1)
			.setLevel(vk::CommandBufferLevel::ePrimary);//һ��buffer���Կ��Ʊ��commandbuffer�����Ʊ��˵ľ���primary
		commandbuffer = Context::GetInstance().device.allocateCommandBuffers(allocateInfo)[0];

		//������cpu�����ݴ���gpu��commandbuffer
		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		commandbuffer.begin(beginInfo);
		{
			vk::BufferCopy region;
			region.setSize(sizeof(verticies))
				.setSrcOffset(0)
				.setDstOffset(0);
			commandbuffer.copyBuffer(hostVertexBuffer_->buffer, deviceVertexBuffer_->buffer, region);

		}
		commandbuffer.end();

		//������
		vk::SubmitInfo submitInfo;
		submitInfo.setCommandBuffers(commandbuffer);
		Context::GetInstance().graphicsQueue.submit(submitInfo);

		Context::GetInstance().device.waitIdle();//һֱ��


		Context::GetInstance().device.freeCommandBuffers(commandPool, commandbuffer);


	}



}