#pragma once
#include <vulkan/vulkan.h>
#include <renderer/vulkanbackend/VulkanContext.h>
#include <renderer/vulkanbackend/VulkanSwapchain.h>
#include <array>
#include <vector>
#include <fstream>
#include <cstring>

static constexpr int s_MAX_FRAMES_IN_FLIGHT{2};

class VulkanAllocator
{
private:
    //objects
    VkCommandPool m_cmdPool = VK_NULL_HANDLE;
    VulkanSwapchain* swapchain;
    VulkanContext* ctx;
    VmaAllocation vBufferAlloc = VK_NULL_HANDLE;

public:
    //objects
    struct ShaderData
    {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
    }shaderData{};

    struct ShaderDataBuffer
    {
        VmaAllocation allocation{ VK_NULL_HANDLE };
        VmaAllocationInfo allocationInfo{};
        VkBuffer buffer{ VK_NULL_HANDLE };
        VkDeviceAddress deviceAddress{};
    };
    std::array<VkFence, s_MAX_FRAMES_IN_FLIGHT> m_fences;
    std::array<VkSemaphore, s_MAX_FRAMES_IN_FLIGHT> m_presentSemaphores;
    std::array<VkCommandBuffer, s_MAX_FRAMES_IN_FLIGHT> m_cmdBuffers;
    std::vector<VkSemaphore> m_renderSemaphores;
    std::array<ShaderDataBuffer, s_MAX_FRAMES_IN_FLIGHT> m_shaderDataBuffers;

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
    void allocateBuffer(VkBuffer& Buffer, VmaAllocation& vmaAlloc);
    void freeBufferMemory(VkBuffer& Buffer, VmaAllocation& vmaAlloc);
    void allocShaderDataBuffer();
};