#include "renderProcess.hpp"

#include "shader.hpp"
#include "swapchain.hpp"
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

		//9.layout��renderpass��vulkan��������
		createInfo.setLayout(layout)
			.setRenderPass(renderPass);

		//10.����pipline
		auto result = Context::GetInstance().device.createGraphicsPipeline(nullptr,createInfo);
		if (result.result != vk::Result::eSuccess) {
			throw std::runtime_error("create pipline failed");
		}
		pipline = result.value;
	}

	void RenderProcess::InitLayout() {//����uniform�Ĳ��֣�
		vk::PipelineLayoutCreateInfo createInfo;
		layout = Context::GetInstance().device.createPipelineLayout(createInfo);

	}

	void RenderProcess::InitrenderPass() {
		vk::RenderPassCreateInfo createInfo;
		//1.attachment
		vk::AttachmentDescription attachment;//������
		attachment.setFormat(Context::GetInstance().swapchain->info.imageFormat.format)//���������ͣ���Ҫ�õ�swapchainһ����image
			.setInitialLayout(vk::ImageLayout::eUndefined)//�����Ĳ��֣�����
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)//��ȥ�Ĳ��֣��Ż�����ɫ����-��ΪҪ��Ⱦ
			.setLoadOp(vk::AttachmentLoadOp::eClear)//���ؽ����Ĳ��������
			.setStoreOp(vk::AttachmentStoreOp::eStore)//���������ô�洢��
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)//��������Ӧ��ģ�建�壿
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setSamples(vk::SampleCountFlagBits::e1);//���������ÿ���㶼����
		createInfo.setAttachments(attachment);
		//2.subpass
		vk::SubpassDescription subpass;
		vk::AttachmentReference refrence;//���������
		refrence.setLayout(vk::ImageLayout::eColorAttachmentOptimal)//��ɫ��layout��layout������ɶ����
			.setAttachment(0);//��Ϊ����setAttachments���������飬�������ľ����������ı��
		subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)//subpass����ͼ���pipline��
			.setColorAttachments(refrence);
		createInfo.setSubpassCount(1)
			.setSubpasses(subpass);
		//3.subpass dependency
		vk::SubpassDependency dependency;//���ж��subpassʱsubpassִ�е�˳����Ϊ��vulkan�Դ���initsubpass���Ա���ָ��
		dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)//��ʹ���ⲿ��subpass
			.setDstSubpass(0)//�����Լ���subpass,0ͬ����������±�
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)//Ȩ�޼�����дcolor��Ȩ��
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		createInfo.setDependencies(dependency);

		renderPass = Context::GetInstance().device.createRenderPass(createInfo);
	};

	RenderProcess::~RenderProcess(){
		Context::GetInstance().device.destroyRenderPass(renderPass);
		Context::GetInstance().device.destroyPipelineLayout(layout);
		Context::GetInstance().device.destroyPipeline(pipline);
	}

}