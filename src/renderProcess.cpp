#include "renderProcess.hpp"

#include "shader.hpp"
#include "context.hpp"

namespace toy2d {
	void RenderProcess::InitPipline(int width,int height) {
		vk::GraphicsPipelineCreateInfo createInfo;

		//1.��������
		vk::PipelineVertexInputStateCreateInfo vertInfo;
		createInfo.setPVertexInputState(&vertInfo);
		
		//2.ȷ����������ʲôͼԪ
		vk::PipelineInputAssemblyStateCreateInfo assemblyInfo;
		assemblyInfo.setPrimitiveRestartEnable(false)
			.setTopology(vk::PrimitiveTopology::eTriangleList);//����������
		createInfo.setPInputAssemblyState(&assemblyInfo);
		
		//3.shader
		std::vector<vk::PipelineShaderStageCreateInfo> shaderInfos;
		shaderInfos.resize(2);
		shaderInfos[0].setStage(vk::ShaderStageFlagBits::eVertex)//������ɫ��
			.setModule(Shader::GetInstance().vertModule)//���붨����ɫ��
			.setPName("main");//��ں�����main
		shaderInfos[1].setStage(vk::ShaderStageFlagBits::eFragment)//������ɫ��
			.setModule(Shader::GetInstance().fragModule)//����Ƭ����ɫ��
			.setPName("main");//��ں�����main
		createInfo.setStages(shaderInfos);

		//4.viewport
		vk::PipelineViewportStateCreateInfo viewportInfo;
		vk::Viewport viewport(0,0,width,height,0,1);
		vk::Rect2D rect2d({0,0},{(uint32_t)width,(uint32_t)height});
		viewportInfo.setViewports(viewport)//һ��viewport
			.setScissors(rect2d);
		createInfo.setPViewportState(&viewportInfo);

		//5.��դ��
		vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
		rasterizationInfo.setRasterizerDiscardEnable(false)//�Ƿ�������դ�����
			.setCullMode(vk::CullModeFlagBits::eBack)//�޳�����
			.setFrontFace(vk::FrontFace::eCounterClockwise)//��ʱ��Ϊ����
			.setPolygonMode(vk::PolygonMode::eFill)//��դ��ʱ����ͼ��
			.setLineWidth(1);

		createInfo.setPRasterizationState(&rasterizationInfo);

		//6.���ز���
		vk::PipelineMultisampleStateCreateInfo multisampleInfo;
		multisampleInfo.setSampleShadingEnable(false)//�����ز���
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);
			createInfo.setPMultisampleState(&multisampleInfo);

		//7.����
			
		//8.��Ⱦ��ɫ
		vk::PipelineColorBlendStateCreateInfo colorblendeingInfo;
		vk::PipelineColorBlendAttachmentState attachments;
		attachments.setBlendEnable(false)//����ɫ�ڻ�
			.setColorWriteMask(
				vk::ColorComponentFlagBits::eA |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eR  );//?
		colorblendeingInfo.setLogicOpEnable(false)//��ɫ�ڻ�ر�
			.setAttachments(attachments); 
		createInfo.setPColorBlendState(&colorblendeingInfo);

		//layout��renderpass��vulkan��������

		//9.����pipline
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