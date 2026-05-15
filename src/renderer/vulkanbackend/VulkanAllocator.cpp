#include <renderer/vulkanbackend/VulkanAllocator.h>


VulkanAllocator::VulkanAllocator(VulkanContext* ctx, VulkanSwapchain* swapchain)
    :ctx(ctx), swapchain(swapchain)
{
    createCommandPool();
    allocateCommandBuffer();
    createSyncObjects();
}

VulkanAllocator::~VulkanAllocator()
{
    for (size_t i{0}; i < s_MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (m_fences[i] != VK_NULL_HANDLE)
            vkDestroyFence(ctx->getDeviceHandle(), m_fences[i], nullptr);

        if (m_presentSemaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(ctx->getDeviceHandle(), m_presentSemaphores[i], nullptr);
    }

    for (const auto& semaphore : m_renderSemaphores)
    {
        if (semaphore != VK_NULL_HANDLE)
            vkDestroySemaphore(ctx->getDeviceHandle(), semaphore, nullptr);
    }

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
        .commandPool = m_cmdPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = s_MAX_FRAMES_IN_FLIGHT
    };

    if (vkAllocateCommandBuffers(ctx->getDeviceHandle(), &cmdBufferAllocCI, m_cmdBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to alloc Command Buffers");
    }
}

void VulkanAllocator::createSyncObjects()
{
    VkFenceCreateInfo FenceCI
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    VkSemaphoreCreateInfo semaphoreCI
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    for (auto i = 0; i < s_MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateFence(ctx->getDeviceHandle(), &FenceCI, nullptr, &m_fences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to created fences");
        }
        if (vkCreateSemaphore(ctx->getDeviceHandle(), &semaphoreCI, nullptr, &m_presentSemaphores[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create present Semaphores");
        }
    }

    const auto& images = swapchain->getSwapchainImages();
    m_renderSemaphores.resize(images.size());
    for (auto& semaphore : m_renderSemaphores)
    {

        if (vkCreateSemaphore(ctx->getDeviceHandle(), &semaphoreCI, nullptr, &semaphore) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create RenderSemaphores");
        }
    }
}

std::vector<char> VulkanAllocator::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary); //öffnet datei binär und mit einem pointer aufs ende der dtaei
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file!");
    }
    //tellg gibt aktuelle poistion des datei pointers an, also das ende, das entspricht der dateigröße, welche wir der variable zuweisen
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    //datei pointer wieder an den anfang setzen
    file.seekg(0);
    //einlesen
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VkShaderModule VulkanAllocator::createShaderModule(const std::vector<char>& code)
{

    VkShaderModuleCreateInfo shaderModuleCI
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t*>(code.data()), //nachgucken
    };
    VkShaderModule shaderModule = VK_NULL_HANDLE;

    if (vkCreateShaderModule(ctx->getDeviceHandle(), &shaderModuleCI, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Shader Module");
    }

    return shaderModule;

}

void VulkanAllocator::allocateBuffer(VkBuffer& Buffer, VmaAllocation& vmaAlloc)
{
    VkDeviceSize vBuffSize{sizeof(VulkanContext::Vertex) * ctx->vertices.size()};
    VkBufferCreateInfo bufferCI
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = vBuffSize,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    };
    VmaAllocationCreateInfo vmaAllocCI
    {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    VmaAllocationInfo vBufferAllocInfo
    {

    };

    if (vmaCreateBuffer(ctx->getAllocatorHandle(), &bufferCI, &vmaAllocCI, &Buffer, &vmaAlloc, &vBufferAllocInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to alloc vBuffer");
    }

    memcpy(vBufferAllocInfo.pMappedData, ctx->vertices.data(), vBuffSize);
}

void VulkanAllocator::freeBufferMemory(VkBuffer& Buffer, VmaAllocation& vmaAlloc)
{
    vmaDestroyBuffer(ctx->getAllocatorHandle(), Buffer, vmaAlloc);
}
