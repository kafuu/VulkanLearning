//上下文，与vulkan所有渲染器有关都保存在这里
#pragma once

#include "vulkan/vulkan.hpp"
#include <memory>
#include <iostream>
#include<optional>
#include<functional>


namespace toy2d {

	using CreateSurfaceFunc = std::function<vk::SurfaceKHR(vk::Instance)>;

	class Context final {
	public:
		//单例
		static Context& GetInstance();
		static void Init(std::vector<const char*> extensions,CreateSurfaceFunc func);
		static void Quit();
		~Context();

		vk::Instance instance;
		vk::PhysicalDevice phyDevice;//物理设备
		vk::Device device;//逻辑设备
		vk::Queue graphicsQueue;
		vk::SurfaceKHR surface;
		struct  QueueFamilyIndices final
		{
			std::optional<uint32_t> graphicsQuene;
		};
		QueueFamilyIndices queueFamilyIndices;

	private:
		Context(std::vector<const char*> extensions,CreateSurfaceFunc func);
		static std::unique_ptr<Context> instance_;//智能指针，是这个类的一个实例，此时只是声明还未实例化
		void createInstance(std::vector<const char*> extensions);
		void pickupPhysicalDevice();
		void createDevice();
		void queryQueueFamilyIndices();
		void getQueues();
	};

}