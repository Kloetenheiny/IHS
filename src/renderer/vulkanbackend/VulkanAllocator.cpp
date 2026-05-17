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

VulkanAllocator::BufferAllocation VulkanAllocator::allocBuffer(VkDeviceSize size, VkBufferUsageFlags usageflags, VmaMemoryUsage memoryusage, VmaAllocationCreateFlags vmaallocflags, const void* data)
{
    BufferAllocation result{};

    VkBufferCreateInfo bufferCI
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usageflags
    };

    VmaAllocationCreateInfo vmaAllocCI
    {
        .flags = vmaallocflags,
        .usage = memoryusage
    };

    if (vmaCreateBuffer(ctx->getAllocatorHandle(), &bufferCI, &vmaAllocCI, &result.m_Buffer, &result.m_vmaAlloc, &result.m_allocationInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to alloc buffer");
    }

    if (data != nullptr)
    {
        if (result.m_allocationInfo.pMappedData)
        {
            memcpy(result.m_allocationInfo.pMappedData, data, size);
        }
        else
        {
            void* mapped = nullptr;
            vmaMapMemory(ctx->getAllocatorHandle(), result.m_vmaAlloc, &mapped);
            memcpy(mapped, data, size);
            vmaUnmapMemory(ctx->getAllocatorHandle(), result.m_vmaAlloc);
        }
    }

    if (usageflags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
    {
        VkBufferDeviceAddressInfo deviceAdressCI
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .buffer = result.m_Buffer
        };

        result.m_deviceAdress = vkGetBufferDeviceAddress(ctx->getDeviceHandle(), &deviceAdressCI);
    }

    return result;
}

void VulkanAllocator::freeBufferMemory(BufferAllocation& allocatedBuffer)
{
    vmaDestroyBuffer(ctx->getAllocatorHandle(), allocatedBuffer.m_Buffer, allocatedBuffer.m_vmaAlloc);
    allocatedBuffer.m_Buffer = VK_NULL_HANDLE;
    allocatedBuffer.m_vmaAlloc = VK_NULL_HANDLE;
}

VulkanAllocator::Image VulkanAllocator::loadImageFromFile(char const* filename)
{
    Image ImageResult{};

    int x{};
    int y{};
    int channels{};

    unsigned char* data = stbi_load(filename, &x, &y, &channels, STBI_rgb_alpha);

    if (!data)
    {
        throw std::runtime_error("Failed to load image");
    }

    VkImageCreateInfo imageCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_B8G8R8A8_SRGB,
        .extent = {.width = static_cast<uint32_t>(x), .height = static_cast<uint32_t>(y), .depth = 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED

    };

    VmaAllocationCreateInfo texAllocCI
    {
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    if (vmaCreateImage(ctx->getAllocatorHandle(), &imageCI, &texAllocCI, &ImageResult.m_TexImage, &ImageResult.m_TexImageVMAAlloc, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Texture Image");
    }

    VkImageViewCreateInfo imageViewCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = ImageResult.m_TexImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = imageCI.format,
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }, //level count??
    };

    if (vkCreateImageView(ctx->getDeviceHandle(), &imageViewCI, nullptr, &ImageResult.m_TexImageView) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create image views");
    }

}

VulkanAllocator::Image VulkanAllocator::createImage()
{

}
