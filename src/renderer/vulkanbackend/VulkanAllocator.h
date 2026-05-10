#pragma once
#include <vulkan/vulkan.h>
#include <renderer/vulkanbackend/VulkanContext.h>

class VulkanAllocator
{
private:
    //objects
    VkCommandPool m_cmdPool = VK_NULL_HANDLE;
    VulkanContext* ctx;

public:
    //objects
private:
    //functions
    void createCommandPool();
    void allocateCommandBuffer();
public:
    //functions
    VulkanAllocator(VulkanContext* ctx);
    ~VulkanAllocator();
};