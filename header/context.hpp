//�����ģ���vulkan������Ⱦ���йض�����������
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
		//����
		static Context& GetInstance();
		static void Init(std::vector<const char*> extensions,CreateSurfaceFunc func);
		static void Quit();
		~Context();

		vk::Instance instance;
		vk::PhysicalDevice phyDevice;//�����豸
		vk::Device device;//�߼��豸
		vk::Queue graphicsQueue;
		vk::SurfaceKHR surface;
		struct  QueueFamilyIndices final
		{
			std::optional<uint32_t> graphicsQuene;
			std::optional<uint32_t> presentQuene;
		};
		QueueFamilyIndices queueFamilyIndices;

	private:
		Context(std::vector<const char*> extensions,CreateSurfaceFunc func);
		static std::unique_ptr<Context> instance_;//����ָ�룬��������һ��ʵ������ʱֻ��������δʵ����
		void createInstance(std::vector<const char*> extensions);
		void pickupPhysicalDevice();
		void createDevice();
		void queryQueueFamilyIndices();
		void getQueues();
	};

}