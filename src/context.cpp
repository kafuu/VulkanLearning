#include "context.hpp"

namespace toy2d {
	std::unique_ptr<Context> Context::instance_ = nullptr;

	void Context::Init(std::vector<const char*> extensions,CreateSurfaceFunc func) {
		instance_.reset(new Context(extensions,func));
	}

	void Context::Quit() {
		instance_.reset();
	}

	Context& Context::GetInstance() {
		assert(instance_);
		return *instance_;
	}

	void Context::InitSwapchain(int w,int h) {

		swapchain.reset(new Swapchain(w, h));
	}

	void Context::DestroySwapchain() {
		swapchain.reset();//先销毁swapchain在销毁device
	}

	Context::Context(std::vector<const char*> extensions,CreateSurfaceFunc func) {
		createInstance(extensions);
		pickupPhysicalDevice();
		surface = func(instance);
		queryQueueFamilyIndices();
		createDevice();
		getQueues();
	}

	Context::~Context() {
		device.destroy();
		instance.destroy();
	}
	//创建instance
	void Context::createInstance(std::vector<const char*> extensions) {
		vk::InstanceCreateInfo createInfo;//创建instance的info:结构体
		vk::ApplicationInfo appInfo;
		//验证层
		std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };

		appInfo.setApiVersion(VK_API_VERSION_1_3);
		createInfo.setPApplicationInfo(&appInfo)
			.setPEnabledLayerNames(layers)
			.setPEnabledExtensionNames(extensions);

		instance = vk::createInstance(createInfo);

	}
	//挑选显卡
	void Context::pickupPhysicalDevice(){
		auto devices = instance.enumeratePhysicalDevices();
		phyDevice = devices[0];
		std::cout << phyDevice.getProperties().deviceName<<std::endl;
	};
	//创建逻辑设备
	void Context::createDevice() {
		std::array extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		vk::DeviceCreateInfo createInfo;
		std::vector <vk::DeviceQueueCreateInfo> queueCreateInfos;
		float priorities = 1.0f;
		if (queueFamilyIndices.presentQuene.value() == queueFamilyIndices.graphicsQuene.value()) {//如果两个queue是同一个
			vk::DeviceQueueCreateInfo queueCreateInfo;
			queueCreateInfo.setPQueuePriorities(&priorities)//优先级
				.setQueueCount(1)//队列数
				.setQueueFamilyIndex(queueFamilyIndices.graphicsQuene.value());//不同的设备不同，因此需要查询
			queueCreateInfos.push_back(std::move(queueCreateInfo));
		}
		else {
			vk::DeviceQueueCreateInfo queueCreateInfo;
			queueCreateInfo.setPQueuePriorities(&priorities)//优先级
				.setQueueCount(1)//队列数
				.setQueueFamilyIndex(queueFamilyIndices.graphicsQuene.value());//不同的设备不同，因此需要查询
			queueCreateInfos.push_back(queueCreateInfo);

			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)
				.setQueueFamilyIndex(queueFamilyIndices.presentQuene.value());
			queueCreateInfos.push_back(queueCreateInfo);
		}
		createInfo.setQueueCreateInfos(queueCreateInfos)//命令队列，用于从device到phydevice传指令
			.setPEnabledExtensionNames(extensions);
		device = phyDevice.createDevice(createInfo);
	};
	//请求到底哪个图形队列是命令队列
	void Context::queryQueueFamilyIndices() {
		auto properties = phyDevice.getQueueFamilyProperties();
		for (int i = 0; i < properties.size();i++) {
			auto property = properties[i];
			if (property.queueFlags | vk::QueueFlagBits::eGraphics) {
				queueFamilyIndices.graphicsQuene = i; 
			}
			if (phyDevice.getSurfaceSupportKHR(i, surface)) {
				queueFamilyIndices.presentQuene = i;
			}
			if (queueFamilyIndices) {
				break;
			}
		}
	}
	//
	void Context::getQueues() {
		graphicsQueue = device.getQueue(queueFamilyIndices.graphicsQuene.value(),0);
		presentQueue = device.getQueue(queueFamilyIndices.presentQuene.value(),0);

	}
}