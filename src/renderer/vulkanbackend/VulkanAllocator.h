#pragma once
#include <vulkan/vulkan.h>
#include <renderer/vulkanbackend/VulkanContext.h>
#include <renderer/vulkanbackend/VulkanSwapchain.h>
#include <array>
#include <vector>
#include <fstream>
#include <cstring>
#include <stb_image/stb_image.h>

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
    struct ShaderData
    {
        glm::mat4 m_projection;
        glm::mat4 m_view;
        glm::mat4 m_model;
    }shaderData{};

    struct BufferAllocation
    {
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_vmaAlloc = VK_NULL_HANDLE;
        VmaAllocationInfo m_allocationInfo{};
        VkDeviceAddress m_deviceAdress{};
    };

    struct Image
    {
        VkImage m_TexImage = VK_NULL_HANDLE;
        VkImageView m_TexImageView = VK_NULL_HANDLE;
        VkDeviceSize m_TexImageSize{};
        VkDeviceMemory m_TexImageDeviceMemory{};
        VmaAllocation m_TexImageVMAAlloc = VK_NULL_HANDLE;
        VkSampler m_TexImageSampler = VK_NULL_HANDLE;
    };

    std::array<VkFence, s_MAX_FRAMES_IN_FLIGHT> m_fences;
    std::array<VkSemaphore, s_MAX_FRAMES_IN_FLIGHT> m_presentSemaphores;
    std::array<VkCommandBuffer, s_MAX_FRAMES_IN_FLIGHT> m_cmdBuffers;
    std::vector<VkSemaphore> m_renderSemaphores;


private:
    //functions
    void createCommandPool();
    void allocateCommandBuffer();
    void createSyncObjects();
    Image loadImageFromFile(char const* filename);
    Image createImage();


public:
    //functions
    VulkanAllocator(VulkanContext* ctx, VulkanSwapchain* swapchain);
    ~VulkanAllocator();
    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    BufferAllocation allocBuffer(VkDeviceSize size, VkBufferUsageFlags usageflags, VmaMemoryUsage memoryusage, VmaAllocationCreateFlags vmaallocflags, const void* = nullptr);
    void freeBufferMemory(BufferAllocation& allocatedBuffer);

};