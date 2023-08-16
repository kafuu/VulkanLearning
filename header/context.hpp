//�����ģ���vulkan������Ⱦ���йض�����������
#pragma once

#include "vulkan/vulkan.hpp"
#include <memory>
#include <iostream>
#include<optional>

namespace toy2d {
	class Context final {
	public:
		//����
		static Context& GetInstance();
		static void Init();
		static void Quit();
		~Context();

		vk::Instance instance;
		vk::PhysicalDevice phyDevice;//�����豸
		vk::Device device;//�߼��豸
		struct  QueueFamilyIndices final
		{
			std::optional<uint32_t> graphicsQuene;
		};
		QueueFamilyIndices queueFamilyIndices;
		vk::Queue graphicsQueue;

	private:
		Context();
		static std::unique_ptr<Context> instance_;//����ָ�룬��������һ��ʵ������ʱֻ��������δʵ����
		void createInstance();
		void pickupPhysicalDevice();
		void createDevice();
		void queryQueueFamilyIndices();
		void getQueues();
	};

}