//上下文，与vulkan所有渲染器有关都保存在这里
#pragma once

#include "vulkan/vulkan.hpp"
#include <memory>
#include <iostream>

namespace toy2d {
	class Context final {
	public:
		//单例
		static Context& GetInstance();
		static void Init();
		static void Quit();
		~Context();
		vk::Instance instance;
	private:
		Context();
		static std::unique_ptr<Context> instance_;//智能指针，是这个类的一个实例，此时只是声明还未实例化
	};

}