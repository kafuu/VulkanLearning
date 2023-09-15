#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {
    struct MemoryInfo final {
        size_t size;
        uint32_t index;
    };
    class Buffer final {
    public:
        vk::Buffer buffer;
        vk::DeviceMemory memory;
        size_t size;

        Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property);
        ~Buffer();

    private:
        void createBuffer(size_t size, vk::BufferUsageFlags usage);
        void allocateMemory(MemoryInfo info);
        void bindingMemToBuf();
        MemoryInfo querymemoryInfo(vk::MemoryPropertyFlags property);
    };

}