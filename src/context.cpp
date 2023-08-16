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
		vk::InstanceCreateInfo createInfo;//创建instance的info:结构体
		vk::ApplicationInfo appInfo;
		std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };
	
		appInfo.setApiVersion(VK_API_VERSION_1_3);
		createInfo.setPApplicationInfo(&appInfo)
			.setPEnabledLayerNames(layers);

		instance = vk::createInstance(createInfo);

	}
	Context::~Context() {
		instance.destroy();
	}
}