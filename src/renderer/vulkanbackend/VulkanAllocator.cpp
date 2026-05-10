#include <renderer/vulkanbackend/VulkanAllocator.h>


VulkanAllocator::VulkanAllocator(VulkanContext* ctx)
    :ctx(ctx)
{
    createCommandPool();
}

VulkanAllocator::~VulkanAllocator()
{
    vkDestroyCommandPool(ctx->getDeviceHandle(), m_cmdPool, nullptr);
}

void VulkanAllocator::createCommandPool()
{
    VkCommandPoolCreateInfo cmdPoolCI
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = ctx->getGraphicsQueueFamilyIndex(),
    };

    if (vkCreateCommandPool(ctx->getDeviceHandle(), &cmdPoolCI, nullptr, &m_cmdPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command Pool");
    }
}

void VulkanAllocator::allocateCommandBuffer()
{
    VkCommandBufferAllocateInfo cmdBufferAllocCI
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_cmdPool

    };
}
