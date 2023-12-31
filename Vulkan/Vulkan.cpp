﻿#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include <iostream>
#include <vector>
#include "toy2d.hpp"

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);


	SDL_Window* window = SDL_CreateWindow("sandbox",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1920, 1080,
		SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
	if (!window) {
		SDL_Log("create window failed");
		exit(2);
	}
	bool shouldClose = false;
	SDL_Event event;
	//传递surface拓展参数给createInstance
	unsigned int count;
	SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
	std::vector<const char*> extensions(count);
	SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data());



	toy2d::Init(extensions,[&](vk::Instance instance) {
			VkSurfaceKHR surface;
			if (!SDL_Vulkan_CreateSurface(window, instance, &surface)) {
				throw std::runtime_error("cant create surface");
			}
			return surface;
		},1920,1080);


	while (!shouldClose) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				shouldClose = true;
			}
		}
		toy2d::GetRenderer().drawTriangle();
	}
	toy2d::Quit();

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}