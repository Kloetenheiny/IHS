#include <renderer/vulkanbackend/VulkanAllocator.h>


VulkanAllocator::VulkanAllocator(VulkanContext* ctx, VulkanSwapchain* swapchain)
    :ctx(ctx), swapchain(swapchain)
{
    createCommandPool();
    allocateCommandBuffer();
    createSyncObjects();
    auto Texture = loadImageFromFile("/home/christian/CLionProjects/IHS/ressources/textures/default_dirt.png");
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

    cleanupDescriptors();
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

VulkanAllocator::Texture VulkanAllocator::loadImageFromFile(char const* filename)
{
    Texture ImageResult{};

    int x{};
    int y{};
    int channels{};

    unsigned char* data = stbi_load(filename, &x, &y, &channels, STBI_rgb_alpha);

    ImageResult.m_TexImageSize = x * y * 4;

    if (!data)
    {
        throw std::runtime_error("Failed to load image");
    }

    VkImageCreateInfo imageCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .extent = {.width = static_cast<uint32_t>(x), .height = static_cast<uint32_t>(y), .depth = 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
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


    auto imgSrcBuffer = allocBuffer(ImageResult.m_TexImageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT, data);


    VkFenceCreateInfo fenceOneTimeCI
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    };
    VkFence fenceOneTime{};

    if (vkCreateFence(ctx->getDeviceHandle(), &fenceOneTimeCI, nullptr, &fenceOneTime) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create fence for image layout transition");
    }

    VkCommandBuffer cbOneTime{};
    VkCommandBufferAllocateInfo cbOneTimeAI{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_cmdPool,
        .commandBufferCount = 1
    };
    if (vkAllocateCommandBuffers(ctx->getDeviceHandle(), &cbOneTimeAI, &cbOneTime) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to alloc cmd buffer for image layout transition");
    }

    VkCommandBufferBeginInfo cbOneTimeBI
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    vkBeginCommandBuffer(cbOneTime, &cbOneTimeBI);

    VkImageMemoryBarrier2 barrierTexImage
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
        .srcAccessMask = VK_ACCESS_2_NONE,
        .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        .dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .image = ImageResult.m_TexImage,
        .subresourceRange = { .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
    };

    VkDependencyInfo barrierTexInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrierTexImage
    };

    vkCmdPipelineBarrier2(cbOneTime, &barrierTexInfo);

    VkBufferImageCopy copyRegion
    {
        .bufferOffset      = 0,          // Offset im Buffer wo die Pixeldaten beginnen
        .bufferRowLength   = 0,          // 0 = dicht gepackt (kein padding zwischen Zeilen)
        .bufferImageHeight = 0,          // 0 = dicht gepackt

        .imageSubresource  =
        {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel       = 0,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        },

        .imageOffset = { 0, 0, 0 },      // Wohin im Image kopiert wird (Startpunkt)
        .imageExtent = { (uint32_t)x, (uint32_t)y, 1 }, // Breite, Höhe, Tiefe (2D = 1)
    };

    vkCmdCopyBufferToImage(cbOneTime, imgSrcBuffer.m_Buffer, ImageResult.m_TexImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

    VkImageMemoryBarrier2 barrierTexRead
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
        .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
        .image = ImageResult.m_TexImage,
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
    };

    barrierTexInfo.pImageMemoryBarriers = &barrierTexRead;
    vkCmdPipelineBarrier2(cbOneTime, &barrierTexInfo);
    vkEndCommandBuffer(cbOneTime);
    VkSubmitInfo oneTimeSI
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &cbOneTime
    };

    vkQueueSubmit(ctx->getGraphicsQueueHandle(), 1, &oneTimeSI, fenceOneTime);
    vkWaitForFences(ctx->getDeviceHandle(), 1, &fenceOneTime, VK_TRUE, UINT64_MAX);

    freeBufferMemory(imgSrcBuffer);
    vkFreeCommandBuffers(ctx->getDeviceHandle(), m_cmdPool, 1, &cbOneTime);
    vkDestroyFence(ctx->getDeviceHandle(), fenceOneTime, nullptr);
    stbi_image_free(data);

    VkSamplerCreateInfo samplerCI
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_NEAREST,
        .minFilter = VK_FILTER_NEAREST,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = 8.0f, // 8 is a widely supported value for max anisotropy
        .minLod = 1,
        .maxLod = VK_LOD_CLAMP_NONE,
    };

    if (vkCreateSampler(ctx->getDeviceHandle(), &samplerCI, nullptr, &ImageResult.m_TexImageSampler) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Image Sampler");
    }

    createDescriptorSet(ImageResult);

    return ImageResult;
}

void VulkanAllocator::createDescriptorSet(Texture& image)
{
    VkDescriptorBindingFlags descVariableFlag{ VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT };
    //is used to enable a variable number of descriptors in that binding as part of descriptor indexing and is passed via pNext
    //We combine texture images and samplers (see below), so the binding's type needs to be VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    VkDescriptorSetLayoutBindingFlagsCreateInfo descBindingFlags
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .bindingCount = 1,
        .pBindingFlags = &descVariableFlag
    };
    VkDescriptorSetLayoutBinding descLayoutBindingTex
    {
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
    };
    VkDescriptorSetLayoutCreateInfo descLayoutTexCI{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = &descBindingFlags,
        .bindingCount = 1,
        .pBindings = &descLayoutBindingTex
    };
    if (vkCreateDescriptorSetLayout(ctx->getDeviceHandle(), &descLayoutTexCI, nullptr, &m_descriptorSetLayoutTex) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Descriptor Set Layout");
    }

    VkDescriptorPoolSize poolSize{
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1
    };
    VkDescriptorPoolCreateInfo descPoolCI{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize
    };

    if (vkCreateDescriptorPool(ctx->getDeviceHandle(), &descPoolCI, nullptr, &m_descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Descriptor Pool");
    }

    uint32_t variableDescCount{ 1 };
    VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescCountAI
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT,
        .descriptorSetCount = 1,
        .pDescriptorCounts = &variableDescCount
    };
    VkDescriptorSetAllocateInfo texDescSetAlloc
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = &variableDescCountAI,
        .descriptorPool = m_descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &m_descriptorSetLayoutTex
    };
    if (vkAllocateDescriptorSets(ctx->getDeviceHandle(), &texDescSetAlloc, &m_descriptorSetTex) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate Descriptor Sets");
    }

    //The VkDescriptorImageInfo refers to an array of descriptors for the textures we loaded above combined with samplers in pImageInfo.
    //Calling vkUpdateDescriptorSets will put that information in the first (and in our case only) binding slot of the descriptor set.
    VkDescriptorImageInfo textureDescriptors
    {
        .sampler     = image.m_TexImageSampler,
        .imageView   = image.m_TexImageView,
        .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    };

    VkWriteDescriptorSet writeDescSet{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = m_descriptorSetTex,
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &textureDescriptors
    };
    vkUpdateDescriptorSets(ctx->getDeviceHandle(), 1, &writeDescSet, 0, nullptr);
}

void VulkanAllocator::cleanupImage(Texture& image)
{
    if (image.m_TexImageSampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(ctx->getDeviceHandle(), image.m_TexImageSampler, nullptr);
        image.m_TexImageSampler = VK_NULL_HANDLE;
    }

    if (image.m_TexImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(ctx->getDeviceHandle(), image.m_TexImageView, nullptr);
        image.m_TexImageView = VK_NULL_HANDLE;
    }

    if (image.m_TexImage != VK_NULL_HANDLE && image.m_TexImageVMAAlloc != VK_NULL_HANDLE)
    {
        vmaDestroyImage(ctx->getAllocatorHandle(), image.m_TexImage, image.m_TexImageVMAAlloc);
        image.m_TexImage = VK_NULL_HANDLE;
        image.m_TexImageVMAAlloc = VK_NULL_HANDLE;
    }

}

void VulkanAllocator::cleanupDescriptors()
{

    //only call on shutdown
    if (m_descriptorPool != VK_NULL_HANDLE)
    {
        // Descriptor Sets werden automatisch mit dem Pool zerstört
        vkDestroyDescriptorPool(ctx->getDeviceHandle(), m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
        m_descriptorSetTex = VK_NULL_HANDLE;
    }

    if (m_descriptorSetLayoutTex != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(ctx->getDeviceHandle(), m_descriptorSetLayoutTex, nullptr);
        m_descriptorSetLayoutTex = VK_NULL_HANDLE;
    }
}
