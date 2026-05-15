#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <vk_mem_alloc.h>
#include "GLFW/glfw3.h"

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
        {{ 0.0f, -0.5f }, {1.0f, 0.0f, 0.0f}},  // Spitze oben - Rot
        {{ 0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}},  // rechts unten - Grün
        {{-0.5f,  0.5f }, {0.0f, 0.0f, 1.0f}}   // links unten - Blau
    };

    struct PushConstants
    {
        glm::vec2 pos[3];   // 3 × 8 Bytes  = 24 Bytes  (+ 8 Padding = 32)
        glm::vec3 color[3]; // 3 × 16 Bytes = 48 Bytes
    };                      // gesamt:        80 Bytes


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