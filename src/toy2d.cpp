#include "toy2d.hpp"

namespace toy2d{

	void Init(std::vector<const char*> extensions,CreateSurfaceFunc func,int w,int h) {
		Context::Init(extensions,func);
		Context::GetInstance().InitSwapchain(w,h);
		Shader::Init(ReadWholeFile("./vert.spv"), ReadWholeFile("./frag.spv"));
		Context::GetInstance().renderProcess->InitrenderPass(); 
		Context::GetInstance().renderProcess->InitLayout(); 
		Context::GetInstance().swapchain->createFrameBuffers(w, h);
		Context::GetInstance().renderProcess->InitPipline(w,h);
		Context::GetInstance().InitRenderer();
	}
	void Quit() {
		Context::GetInstance().device.waitIdle();
		Context::GetInstance().renderer.reset();
		Context::GetInstance().renderProcess.reset();
		Shader::Quit();
		Context::GetInstance().DestroySwapchain();
		Context::Quit();
		
	}
}