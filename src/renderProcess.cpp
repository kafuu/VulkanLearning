#include "renderProcess.hpp"

#include "shader.hpp"
#include "context.hpp"

namespace toy2d {
	void RenderProcess::InitPipline(int width,int height) {
		vk::GraphicsPipelineCreateInfo createInfo;

		//1.顶点输入
		vk::PipelineVertexInputStateCreateInfo vertInfo;
		createInfo.setPVertexInputState(&vertInfo);
		
		//2.确定顶点连成什么图元
		vk::PipelineInputAssemblyStateCreateInfo assemblyInfo;
		assemblyInfo.setPrimitiveRestartEnable(false)
			.setTopology(vk::PrimitiveTopology::eTriangleList);//画成三角形
		createInfo.setPInputAssemblyState(&assemblyInfo);
		
		//3.shader
		std::vector<vk::PipelineShaderStageCreateInfo> shaderInfos;
		shaderInfos.resize(2);
		shaderInfos[0].setStage(vk::ShaderStageFlagBits::eVertex)//顶点着色器
			.setModule(Shader::GetInstance().vertModule)//传入定点着色器
			.setPName("main");//入口函数是main
		shaderInfos[1].setStage(vk::ShaderStageFlagBits::eFragment)//顶点着色器
			.setModule(Shader::GetInstance().fragModule)//传入片段着色器
			.setPName("main");//入口函数是main
		createInfo.setStages(shaderInfos);

		//4.viewport
		vk::PipelineViewportStateCreateInfo viewportInfo;
		vk::Viewport viewport(0,0,width,height,0,1);
		vk::Rect2D rect2d({0,0},{(uint32_t)width,(uint32_t)height});
		viewportInfo.setViewports(viewport)//一个viewport
			.setScissors(rect2d);
		createInfo.setPViewportState(&viewportInfo);

		//5.光栅化
		vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
		rasterizationInfo.setRasterizerDiscardEnable(false)//是否抛弃光栅化结果
			.setCullMode(vk::CullModeFlagBits::eBack)//剔除背面
			.setFrontFace(vk::FrontFace::eCounterClockwise)//逆时针为正面
			.setPolygonMode(vk::PolygonMode::eFill)//光栅化时填满图形
			.setLineWidth(1);

		createInfo.setPRasterizationState(&rasterizationInfo);

		//6.多重采样
		vk::PipelineMultisampleStateCreateInfo multisampleInfo;
		multisampleInfo.setSampleShadingEnable(false)//不多重采样
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);
			createInfo.setPMultisampleState(&multisampleInfo);

		//7.测试
			
		//8.渲染颜色
		vk::PipelineColorBlendStateCreateInfo colorblendeingInfo;
		vk::PipelineColorBlendAttachmentState attachments;
		attachments.setBlendEnable(false)//不颜色融混
			.setColorWriteMask(
				vk::ColorComponentFlagBits::eA |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eR  );//?
		colorblendeingInfo.setLogicOpEnable(false)//颜色融混关闭
			.setAttachments(attachments); 
		createInfo.setPColorBlendState(&colorblendeingInfo);

		//layout，renderpass是vulkan独有特性

		//9.创建pipline
		auto result = Context::GetInstance().device.createGraphicsPipeline(nullptr,createInfo);
		if (result.result != vk::Result::eSuccess) {
			throw std::runtime_error("create pipline failed");
		}
		pipline = result.value;
	}

	void RenderProcess::DestoryPipline() {
		Context::GetInstance().device.destroyPipeline(pipline);
	}

}