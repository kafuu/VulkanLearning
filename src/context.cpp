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
	//����instance
	void Context::createInstance() {
		vk::InstanceCreateInfo createInfo;//����instance��info:�ṹ��
		vk::ApplicationInfo appInfo;
		std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };

		appInfo.setApiVersion(VK_API_VERSION_1_3);
		createInfo.setPApplicationInfo(&appInfo)
			.setPEnabledLayerNames(layers);

		instance = vk::createInstance(createInfo);

	}
	//��ѡ�Կ�
	void Context::pickupPhysicalDevice(){
		auto devices = instance.enumeratePhysicalDevices();
		phyDevice = devices[0];
		std::cout << phyDevice.getProperties().deviceName<<std::endl;
	};
	//�����߼��豸
	void Context::createDevice() {
		vk::DeviceCreateInfo createInfo;
		vk::DeviceQueueCreateInfo queneInfo;
		float priorities = 1.0f;

		queneInfo.setPQueuePriorities(&priorities)//���ȼ�
			.setQueueCount(1)//������
			.setQueueFamilyIndex(queueFamilyIndices.graphicsQuene.value());//��ͬ���豸��ͬ�������Ҫ��ѯ
		createInfo.setQueueCreateInfos(queneInfo);//������У����ڴ�device��phydevice��ָ��

		device = phyDevice.createDevice(createInfo);
	
	};
	//���󵽵��ĸ�ͼ�ζ������������
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
		graphicsQueue = device.getQueue(queueFamilyIndices.graphicsQuene.value(),0);//�ڶ�������������һ�����У���Ϊ������һ��������0

	}
}