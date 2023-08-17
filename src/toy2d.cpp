#include "toy2d.hpp"

namespace toy2d{

	void Init(std::vector<const char*> extensions,CreateSurfaceFunc func) {
		Context::Init(extensions,func);
	}
	void Quit() {
		Context::Quit();
	}
}