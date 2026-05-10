#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include <array>
#include <vector>
#include <vk_mem_alloc.h>

class VulkanContext
{
private:
    //objects
    VkInstance m_VulkanInstance = VK_NULL_HANDLE;
    VkDevice m_VulkanDevice = VK_NULL_HANDLE;
    VkPhysicalDevice m_VulkanPhysicalDevice = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VmaAllocator m_allocator_instance = VK_NULL_HANDLE;
    std::array<const char*, 1> m_validationLayer{"VK_LAYER_KHRONOS_validation"};
    const std::array<const char*, 1 > m_deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    uint32_t m_graphicsQueueFamilyIndex{};

public:
    //functions
    VulkanContext();
    ~VulkanContext();
    VkInstance getInstanceHandle() const {return m_VulkanInstance;}
    VkDevice getDeviceHandle() const {return m_VulkanDevice;}
    VkPhysicalDevice getPhysicalDeviceHandle() const {return m_VulkanPhysicalDevice;}
    VkQueue getGraphicsQueueHandle() const {return m_graphicsQueue;}
    uint32_t getGraphicsQueueFamilyIndex() const {return m_graphicsQueueFamilyIndex;}


private:
    //functions
    void createInstance();
    void createDevice();
    void createAllocator();
    bool CheckValidationLayerSupport();
    uint32_t findGraphicsQueueFamilyIndex();

public:
    //objects
};