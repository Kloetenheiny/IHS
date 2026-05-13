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
    void createGraphicsPipeline();
public:
    //functions
    VulkanGraphicsPipeline(VulkanContext* ctx, VulkanAllocator* vlknAlloc);
    ~VulkanGraphicsPipeline();
    VkPipeline getGraphicsPipelineHandle() const {return m_VulkanGraphicsPipeline;}
    VkPipelineLayout getGraphicsPipelineLayoutHandle() const {return m_VulkanGraphicsPipelinelayout;}
};
