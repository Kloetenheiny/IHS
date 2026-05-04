#pragma once
#include <vulkan/vulkan.h>

class VulkanContext
{
private:
    //objects
    VkInstance m_VulkanInstance = VK_NULL_HANDLE;
    VkDevice m_VulkanDevice = VK_NULL_HANDLE;
    VkPhysicalDevice m_VulkanPhysicalDevice = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
public:
    //functions
    VulkanContext() = default;
    ~VulkanContext();
    VkInstance getInstanceHandle() const {return m_VulkanInstance;}
    VkDevice getDeviceHandle() const {return m_VulkanDevice;}
    VkPhysicalDevice getPhysicalDeviceHandle() const {return m_VulkanPhysicalDevice;}
    uint32_t getGraphicsQueueHandle() const {return m_graphicsQueue;}

private:
    //functions
    void createInstance();
public:
    //objects
};