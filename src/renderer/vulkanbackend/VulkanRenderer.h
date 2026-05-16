#pragma once
#include <renderer/vulkanbackend/VulkanContext.h>
#include <windowsystem/IWindow.h>
#include <renderer/vulkanbackend/VulkanSwapchain.h>
#include <renderer/vulkanbackend/VulkanAllocator.h>
#include <renderer/vulkanbackend/VulkanPipeline.h>
#include <chrono>

class VulkanRenderer
{
private:
    //objects
    VulkanContext Context{};
    IWindow window{&Context};
    VulkanSwapchain Swapchain{&Context, &window};
    VulkanAllocator Allocator{&Context, &Swapchain};
    VulkanGraphicsPipeline GraphicsPipeline{&Context, &Allocator};
    uint32_t frameIndex{0};
    uint32_t imageIndex{0};
    VmaAllocation vmaAlloc{};
    VmaAllocation iBuffvmaAlloc{};
    VkBuffer vBuffer = VK_NULL_HANDLE;
    VkBuffer iBuffer = VK_NULL_HANDLE;
public:
    //objects
private:
    //functins
public:
    //functions
    VulkanRenderer();
    ~VulkanRenderer();
    void draw();
};
