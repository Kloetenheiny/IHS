#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>
#include <renderer/vulkanbackend/VulkanContext.h>
#include <renderer/vulkanbackend/VulkanAllocator.h>

class VulkanGraphicsPipeline
{
private:
    //objects
    VkPipeline m_VulkanGraphicsPipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_VulkanGraphicsPipelinelayout = VK_NULL_HANDLE;
    VkShaderModule m_VertShaderModule = VK_NULL_HANDLE;
    VkShaderModule m_FragShaderModule = VK_NULL_HANDLE;
    VulkanAllocator* vlknAlloc;
    VulkanContext* ctx;
public:
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
        {{ 0.0f, -0.5f }, {1.0f, 0.0f, 0.0f}}, // unten Mitte - Rot
        {{ 0.5f,  0.5f }, {0.0f, 1.0f, 0.0f}}, // rechts oben - Grün
        {{-0.5f,  0.5f }, {0.0f, 0.0f, 1.0f}}  // links oben - Blau
    };
private:
    //functions
    void createGraphicsPipeline();
public:
    //functions
    VulkanGraphicsPipeline(VulkanContext* ctx, VulkanAllocator* vlknAlloc);
    ~VulkanGraphicsPipeline();
};
