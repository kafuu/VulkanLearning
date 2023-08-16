//�����ģ���vulkan������Ⱦ���йض�����������
#pragma once

#include "vulkan/vulkan.hpp"
#include <memory>
#include <iostream>

namespace toy2d {
	class Context final {
	public:
		//����
		static Context& GetInstance();
		static void Init();
		static void Quit();
		~Context();
		vk::Instance instance;
	private:
		Context();
		static std::unique_ptr<Context> instance_;//����ָ�룬��������һ��ʵ������ʱֻ��������δʵ����
	};

}