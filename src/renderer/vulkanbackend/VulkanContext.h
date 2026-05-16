#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <vk_mem_alloc.h>
#include "GLFW/glfw3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

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
    VmaAllocator getAllocatorHandle() const {return m_allocator_instance;}

    //objects
    // Position + Farbe pro Vertex
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;
    };

    // Einfaches Dreieck
    const std::vector<Vertex> vertices =
    {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},  // oben links  - Rot
        {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},  // oben rechts - Grün
        {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},  // unten rechts - Blau
        {{-0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}}   // unten links  - Gelb
    };

    const std::vector<uint16_t> indices =
    {
        0, 1, 2,  // erstes Dreieck  (oben links → oben rechts → unten rechts)
        2, 3, 0   // zweites Dreieck (unten rechts → unten links → oben links)
    };

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