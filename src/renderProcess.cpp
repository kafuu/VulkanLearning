#include "renderProcess.hpp"

#include "shader.hpp"
#include "swapchain.hpp"
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

		//9.layout，renderpass是vulkan独有特性
		createInfo.setLayout(layout)
			.setRenderPass(renderPass);

		//10.创建pipline
		auto result = Context::GetInstance().device.createGraphicsPipeline(nullptr,createInfo);
		if (result.result != vk::Result::eSuccess) {
			throw std::runtime_error("create pipline failed");
		}
		pipline = result.value;
	}

	void RenderProcess::InitLayout() {//传入uniform的布局？
		vk::PipelineLayoutCreateInfo createInfo;
		layout = Context::GetInstance().device.createPipelineLayout(createInfo);

	}

	void RenderProcess::InitrenderPass() {
		vk::RenderPassCreateInfo createInfo;
		//1.attachment
		vk::AttachmentDescription attachment;//纹理附件
		attachment.setFormat(Context::GetInstance().swapchain->info.imageFormat.format)//纹理附件类型，需要用到swapchain一样的image
			.setInitialLayout(vk::ImageLayout::eUndefined)//进来的布局：不管
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)//出去的布局：优化的颜色附件-因为要渲染
			.setLoadOp(vk::AttachmentLoadOp::eClear)//加载进来的操作：清空
			.setStoreOp(vk::AttachmentStoreOp::eStore)//点绘制完怎么存储？
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)//纹理附件对应的模板缓冲？
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setSamples(vk::SampleCountFlagBits::e1);//纹理采样：每个点都采样
		createInfo.setAttachments(attachment);
		//2.subpass
		vk::SubpassDescription subpass;
		vk::AttachmentReference refrence;//纹理的引用
		refrence.setLayout(vk::ImageLayout::eColorAttachmentOptimal)//颜色的layout（layout到底是啥？？
			.setAttachment(0);//因为上面setAttachments传的是数组，这里代表的就是这个数组的标号
		subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)//subpass绑定在图像的pipline上
			.setColorAttachments(refrence);
		createInfo.setSubpassCount(1)
			.setSubpasses(subpass);
		//3.subpass dependency
		vk::SubpassDependency dependency;//当有多个subpass时subpass执行的顺序，因为有vulkan自带的initsubpass所以必须指定
		dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)//先使用外部的subpass
			.setDstSubpass(0)//后做自己的subpass,0同理，是数组的下标
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)//权限级：给写color的权限
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