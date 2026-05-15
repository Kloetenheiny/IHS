#pragma once
#include <vulkan/vulkan.h>
#include <vector>

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
