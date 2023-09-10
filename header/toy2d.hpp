#pragma once

#include "vulkan/vulkan.hpp"
#include "shader.hpp"
#include "context.hpp"
#include "tool.hpp"
#include <string>
#include <functional>


namespace toy2d{
	//CreateSurfaceFunc����ģ�壬�൱��һ������
	
	void Init(std::vector<const char*> extensions,CreateSurfaceFunc func,int w,int h);
	void Quit();

	inline Renderer& GetRenderer() {
		return *Context::GetInstance().renderer;
	}
}