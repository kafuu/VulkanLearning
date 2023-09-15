
#include "buffer.hpp"
#include "context.hpp"

namespace toy2d {

	Buffer::Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property) {
		createBuffer(size, usage);
		auto info = querymemoryInfo(property);
		allocateMemory(info);
		bindingMemToBuf();
	}
	Buffer::~Buffer() {
		Context::GetInstance().device.freeMemory(memory);
		Context::GetInstance().device.destroyBuffer(buffer);
	}

	void Buffer::createBuffer(size_t size, vk::BufferUsageFlags usage) {
		vk::BufferCreateInfo createInfo;
		createInfo.setSize(size)
			.setUsage(usage)
			.setSharingMode(vk::SharingMode::eExclusive);//不共用
		buffer = Context::GetInstance().device.createBuffer(createInfo);
	}
	MemoryInfo Buffer::querymemoryInfo(vk::MemoryPropertyFlags property) {
		MemoryInfo info;
		auto requirements = Context::GetInstance().device.getBufferMemoryRequirements(buffer);
		info.size = requirements.size;
		auto properties = Context::GetInstance().phyDevice.getMemoryProperties();
		for (int i = 0; i < properties.memoryTypeCount; i++) {
			if ((1 << i) & requirements.memoryTypeBits &&
				properties.memoryTypes[i].propertyFlags & property) {
				info.index = i;
				break;
			}
		}
		return info;
	}

	void Buffer::allocateMemory(MemoryInfo info) {
		vk::MemoryAllocateInfo alloInfo;
		alloInfo.setMemoryTypeIndex(info.index)//和命令队列的familyindex一样的
			.setAllocationSize(info.size);
		memory = Context::GetInstance().device.allocateMemory(alloInfo);
	}
	void Buffer::bindingMemToBuf() {
		Context::GetInstance().device.bindBufferMemory(buffer, memory, 0);//偏移量0

	}
}