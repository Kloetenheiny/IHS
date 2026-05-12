#pragma once
#include <vulkan/vulkan.h>
#include <renderer/vulkanbackend/VulkanContext.h>
#include <renderer/vulkanbackend/VulkanSwapchain.h>
#include <array>
#include <vector>
#include <fstream>

static constexpr int s_MAX_FRAMES_IN_FLIGHT{2};

class VulkanAllocator
{
private:
    //objects
    VkCommandPool m_cmdPool = VK_NULL_HANDLE;





    VulkanSwapchain* swapchain;
    VulkanContext* ctx;

public:
    //objects
    std::array<VkFence, s_MAX_FRAMES_IN_FLIGHT> m_fences;
    std::array<VkSemaphore, s_MAX_FRAMES_IN_FLIGHT> m_presentSemaphores;
    std::array<VkCommandBuffer, s_MAX_FRAMES_IN_FLIGHT> m_cmdBuffers;
    std::vector<VkSemaphore> m_renderSemaphores;
private:
    //functions
    void createCommandPool();
    void allocateCommandBuffer();
    void createSyncObjects();

public:
    //functions
    VulkanAllocator(VulkanContext* ctx, VulkanSwapchain* swapchain);
    ~VulkanAllocator();
    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);
};