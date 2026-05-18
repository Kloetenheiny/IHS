#include <renderer/vulkanbackend/VulkanRenderer.h>

VulkanRenderer::VulkanRenderer()
{

}

VulkanRenderer::~VulkanRenderer()
{

}

void VulkanRenderer::draw()
{

    auto vertexBuffer = Allocator.allocBuffer(sizeof(VulkanContext::Vertex) * Context.vertices.size(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                    | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT
                    | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                    Context.vertices.data());


    auto indexBuffer = Allocator.allocBuffer(sizeof(uint16_t) * Context.indices.size(),
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VMA_MEMORY_USAGE_AUTO,
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT
                | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                Context.indices.data());

    std::array<VulkanAllocator::BufferAllocation, s_MAX_FRAMES_IN_FLIGHT> shaderDataBuffers;

    for (size_t i{}; i < s_MAX_FRAMES_IN_FLIGHT; i++)
    {
        shaderDataBuffers[i] = Allocator.allocBuffer(
            sizeof(Allocator.shaderData),
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            VMA_MEMORY_USAGE_AUTO,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
            | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT
            | VMA_ALLOCATION_CREATE_MAPPED_BIT
        );
    }

    auto Texture = Allocator.loadImageFromFile("/home/christian/CLionProjects/IHS/ressources/textures/default_dirt.png");

    while (!glfwWindowShouldClose(window.getWindowHandle()))
    {
        vkWaitForFences(Context.getDeviceHandle(), 1, &Allocator.m_fences[frameIndex], true, UINT64_MAX);

        vkResetFences(Context.getDeviceHandle(), 1, &Allocator.m_fences[frameIndex]);

        vkAcquireNextImageKHR(Context.getDeviceHandle(), Swapchain.getSwapchainHandle(), UINT64_MAX, Allocator.m_presentSemaphores[frameIndex], VK_NULL_HANDLE, &imageIndex);

        auto cb = Allocator.m_cmdBuffers[frameIndex];
        vkResetCommandBuffer(cb, 0);

        VkCommandBufferBeginInfo cbCI
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };

        vkBeginCommandBuffer(cb, &cbCI);

        std::array<VkImageMemoryBarrier2, 1> outputBarriers
        {
            VkImageMemoryBarrier2
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = 0,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                .image = Swapchain.getSwapchainImages()[imageIndex],
                .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
            },
        };
        VkDependencyInfo barrierDependencyInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = outputBarriers.data()
        };

        vkCmdPipelineBarrier2(cb, &barrierDependencyInfo);

        VkRenderingAttachmentInfo colorAttachmentInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = Swapchain.getSwapchainImageViews()[imageIndex],
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue{.color{ 0.2f, 0.0f, 0.2f, 1.0f }},
        };


        VkRenderingInfo renderingInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea{.extent{Swapchain.m_swapchainExtent}},
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentInfo,
        };

        vkCmdBeginRendering(cb, &renderingInfo);

        VkViewport vp
        {
            .x = 0.0f,
            .y = static_cast<float>(Swapchain.m_swapchainExtent.height),  // Start unten
            .width  =  static_cast<float>(Swapchain.m_swapchainExtent.width),
            .height = -static_cast<float>(Swapchain.m_swapchainExtent.height),  // negativ!
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
        vkCmdSetViewport(cb, 0, 1, &vp);
        VkRect2D scissor{ .extent{ .width = Swapchain.m_swapchainExtent.width, .height = Swapchain.m_swapchainExtent.height } };
        vkCmdSetScissor(cb, 0, 1, &scissor);

        vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline.getGraphicsPipelineHandle());


        //vkCmdPushConstants(cb, GraphicsPipeline.getGraphicsPipelineLayoutHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VulkanContext::Vertex) * 3, Context.vertices.data());
        vkCmdPushConstants(cb, GraphicsPipeline.getGraphicsPipelineLayoutHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkDeviceAddress), &shaderDataBuffers[frameIndex].m_deviceAdress);

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


        Allocator.shaderData.m_model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        Allocator.shaderData.m_view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        Allocator.shaderData.m_projection = glm::perspective(glm::radians(45.0f), Swapchain.m_swapchainExtent.width / (float)Swapchain.m_swapchainExtent.height, 0.1f, 10.0f);


        memcpy(shaderDataBuffers[frameIndex].m_allocationInfo.pMappedData, &Allocator.shaderData, sizeof(Allocator.shaderData));


        VkDeviceSize vOffset{};
        vkCmdBindVertexBuffers(cb, 0, 1, &vertexBuffer.m_Buffer, &vOffset);
        vkCmdBindIndexBuffer(cb, indexBuffer.m_Buffer, 0, VK_INDEX_TYPE_UINT16);

        //vkCmdDraw(cb, 6, 1, 0, 0);
        vkCmdDrawIndexed(cb, Context.indices.size(), 1, 0, 0, 0);

        vkCmdEndRendering(cb);

        VkImageMemoryBarrier2 barrierPresent
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = 0,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image = Swapchain.getSwapchainImages()[imageIndex],
            .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
        };
        VkDependencyInfo barrierPresentDependencyInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &barrierPresent
        };

        vkCmdPipelineBarrier2(cb, &barrierPresentDependencyInfo);

        vkEndCommandBuffer(cb);

        VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &Allocator.m_presentSemaphores[frameIndex],
            .pWaitDstStageMask = &waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &cb,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &Allocator.m_renderSemaphores[imageIndex],
        };
        vkQueueSubmit(Context.getGraphicsQueueHandle(), 1, &submitInfo, Allocator.m_fences[frameIndex]);

        frameIndex = (frameIndex + 1) % 2;
        auto swapchain = Swapchain.getSwapchainHandle();
        VkPresentInfoKHR presentInfo
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &Allocator.m_renderSemaphores[imageIndex],
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &imageIndex
        };

        vkQueuePresentKHR(Context.getGraphicsQueueHandle(), &presentInfo);


        vkDeviceWaitIdle(Context.getDeviceHandle());

        glfwPollEvents();
    }

    Allocator.freeBufferMemory(vertexBuffer);
    Allocator.freeBufferMemory(indexBuffer);
    for (size_t i{}; i < s_MAX_FRAMES_IN_FLIGHT; i++)
    {
        Allocator.freeBufferMemory(shaderDataBuffers[i]);
    }

    Allocator.cleanupImage(Texture);
}

