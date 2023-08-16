#include "context.hpp"

namespace toy2d {
	std::unique_ptr<Context> Context::instance_ = nullptr;

	void Context::Init() {
		instance_.reset(new Context);
	}

	void Context::Quit() {
		instance_.reset();
	}

	Context& Context::GetInstance() {
		return *instance_;
	}

	Context::Context() {
		createInstance();
		pickupPhysicalDevice();
		queryQueueFamilyIndices();
		createDevice();
		getQueues();
	}

	Context::~Context() {
		device.destroy();
		instance.destroy();
	}
	//创建instance
	void Context::createInstance() {
		vk::InstanceCreateInfo createInfo;//创建instance的info:结构体
		vk::ApplicationInfo appInfo;
		std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };

		appInfo.setApiVersion(VK_API_VERSION_1_3);
		createInfo.setPApplicationInfo(&appInfo)
			.setPEnabledLayerNames(layers);

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
		vk::DeviceCreateInfo createInfo;
		vk::DeviceQueueCreateInfo queneInfo;
		float priorities = 1.0f;

		queneInfo.setPQueuePriorities(&priorities)//优先级
			.setQueueCount(1)//队列数
			.setQueueFamilyIndex(queueFamilyIndices.graphicsQuene.value());//不同的设备不同，因此需要查询
		createInfo.setQueueCreateInfos(queneInfo);//命令队列，用于从device到phydevice传指令

		device = phyDevice.createDevice(createInfo);
	
	};
	//请求到底哪个图形队列是命令队列
	void Context::queryQueueFamilyIndices() {
		auto properties = phyDevice.getQueueFamilyProperties();
		for (int i = 0; i < properties.size();i++) {
			auto property = properties[i];
			if (property.queueFlags | vk::QueueFlagBits::eGraphics) {
				queueFamilyIndices.graphicsQuene = i;
				break;
			}
		}
	}
	//
	void Context::getQueues() {
		graphicsQueue = device.getQueue(queueFamilyIndices.graphicsQuene.value(),0);//第二个参数是拿哪一个队列，因为创建了一个所以是0

	}
}