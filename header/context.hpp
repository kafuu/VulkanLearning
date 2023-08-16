//上下文，与vulkan所有渲染器有关都保存在这里
#pragma once

#include "vulkan/vulkan.hpp"
#include <memory>
#include <iostream>
#include<optional>

namespace toy2d {
	class Context final {
	public:
		//单例
		static Context& GetInstance();
		static void Init();
		static void Quit();
		~Context();

		vk::Instance instance;
		vk::PhysicalDevice phyDevice;//物理设备
		vk::Device device;//逻辑设备
		struct  QueueFamilyIndices final
		{
			std::optional<uint32_t> graphicsQuene;
		};
		QueueFamilyIndices queueFamilyIndices;
		vk::Queue graphicsQueue;

	private:
		Context();
		static std::unique_ptr<Context> instance_;//智能指针，是这个类的一个实例，此时只是声明还未实例化
		void createInstance();
		void pickupPhysicalDevice();
		void createDevice();
		void queryQueueFamilyIndices();
		void getQueues();
	};

}