#include <renderer/vulkanbackend/VulkanPipeline.h>

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanContext* ctx, VulkanAllocator* vlknAlloc)
    :ctx(ctx), vlknAlloc(vlknAlloc)
{
    createGraphicsPipeline();
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
    vkDestroyPipelineLayout(ctx->getDeviceHandle(), m_VulkanGraphicsPipelinelayout, nullptr);
    vkDestroyPipeline(ctx->getDeviceHandle(), m_VulkanGraphicsPipeline, nullptr);
}

void VulkanGraphicsPipeline::createGraphicsPipeline()
{

    auto vertShaderCode = vlknAlloc->readFile("/home/christian/CLionProjects/IHS/ressources/shaders/bin/shader.vert.spv");
    auto fragShaderCode = vlknAlloc->readFile("/home/christian/CLionProjects/IHS/ressources/shaders/bin/shader.frag.spv");

    m_VertShaderModule = vlknAlloc->createShaderModule(vertShaderCode);
    m_FragShaderModule = vlknAlloc->createShaderModule(fragShaderCode);

    VkPushConstantRange pushConstantRange
    {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .size = sizeof(PushConstants)
    };

    VkPipelineLayoutCreateInfo pipelineLayoutCI
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstantRange,
    };

    if (vkCreatePipelineLayout(ctx->getDeviceHandle(), &pipelineLayoutCI, nullptr, &m_VulkanGraphicsPipelinelayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipelinelayout");
    }

    VkVertexInputBindingDescription bindingDescription
    {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    /*std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

    // Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    // Farbe
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);*/

    VkPipelineVertexInputStateCreateInfo vertexInputState
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,

    };


    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };

    VkPipelineShaderStageCreateInfo vertShaderInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = m_VertShaderModule,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo fragShaderInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = m_FragShaderModule,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderInfo, fragShaderInfo };

    //eventuell fehlerhaft weil scissor fehlt
    VkPipelineViewportStateCreateInfo viewportState
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1
    };

    std::vector<VkDynamicState> dynamicStates{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = dynamicStates.data()
    };

    const VkFormat imageFormat{VK_FORMAT_B8G8R8A8_SRGB };
    VkPipelineRenderingCreateInfo renderingCI
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &imageFormat,
    };


    VkPipelineColorBlendAttachmentState blendAttachment
    {
        .colorWriteMask = 0xF //??????
    };

    VkPipelineColorBlendStateCreateInfo colorBlendState
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &blendAttachment
    };

    VkPipelineRasterizationStateCreateInfo rasterizationState
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .lineWidth = 1.0f
   };

    VkPipelineMultisampleStateCreateInfo multisampleState
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
    };


    VkGraphicsPipelineCreateInfo graphicsPioelineCI
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderingCI,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputState,
        .pInputAssemblyState = &inputAssemblyState,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizationState,
        .pMultisampleState = &multisampleState,
        .pColorBlendState = &colorBlendState,
        .pDynamicState = &dynamicState,
        .layout = m_VulkanGraphicsPipelinelayout,
    };

    if (vkCreateGraphicsPipelines(ctx->getDeviceHandle(), VK_NULL_HANDLE, 1, &graphicsPioelineCI, nullptr, &m_VulkanGraphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline");
    }


    vkDestroyShaderModule(ctx->getDeviceHandle(), m_VertShaderModule, nullptr);
    vkDestroyShaderModule(ctx->getDeviceHandle(), m_FragShaderModule, nullptr);

    
}
