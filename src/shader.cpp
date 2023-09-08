#include "shader.hpp"
#include "context.hpp"

namespace toy2d{

	std::unique_ptr<Shader> Shader::instance_ = nullptr;

	void Shader::Init(const std::string& vertSource, const std::string& fragSource) {
		instance_.reset(new Shader(vertSource, fragSource));
	}

	void Shader::Quit() {
		instance_.reset();
	}
	
	Shader::Shader(const std::string& vertSource, const std::string& fragSource) {
		vk::ShaderModuleCreateInfo createInfo;
		createInfo.setCodeSize(vertSource.size())
			.setPCode((uint32_t*)vertSource.data());
		vertModule = Context::GetInstance().device.createShaderModule(createInfo);
		
		createInfo.setCodeSize(fragSource.size())
			.setPCode((uint32_t*)fragSource.data());
		fragModule = Context::GetInstance().device.createShaderModule(createInfo);


	}

	Shader::~Shader() {
		Context::GetInstance().device.destroyShaderModule(vertModule);
		Context::GetInstance().device.destroyShaderModule(fragModule);

	}

	
}