#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {

class Shader final {
public:
	Shader(const std::string& vertSource, const std::string& fragSource);
	~Shader();
	static void Init(const std::string&vertSource,const std::string&fragSource);
	static void Quit();

	static Shader& Shader::GetInstance() {
		return *instance_;
	}

	vk::ShaderModule vertModule;
	vk::ShaderModule fragModule;
private:
	static std::unique_ptr<Shader> instance_;
};

}