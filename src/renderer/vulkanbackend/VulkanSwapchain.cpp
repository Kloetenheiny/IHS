#include <algorithm>
#include <renderer/vulkanbackend/VulkanSwapchain.h>

#include "GLFW/glfw3.h"
//TODO PRESENT MODE COLOR FORMAT AND SPACE
VulkanSwapchain::VulkanSwapchain(VulkanContext* cntx, IWindow* window)
    :cntx(cntx), window(window)
{
    createSwapchain();
}

VulkanSwapchain::~VulkanSwapchain()
{
    for (const auto e : m_SwapchainImageViews)
    {
        vkDestroyImageView(cntx->getDeviceHandle(), e, nullptr);
    }

    vkDestroySwapchainKHR(cntx->getDeviceHandle(), m_VulkanSwapchain, nullptr);
}


void VulkanSwapchain::createSwapchain()
{
    VkSurfaceCapabilitiesKHR surfaceCaps = getSwapchainSurfaceCaps();

    // 1. Korrekte Dimensionen ermitteln
    if (surfaceCaps.currentExtent.width != UINT32_MAX)
    {
        m_swapchainExtent = surfaceCaps.currentExtent;
    }
    else
    {
        // Falls das OS die Größe nicht vorgibt, Framebuffer-Größe vom Window abfragen
        int width, height;
        window->getFramebufferSize(&width, &height);

        m_swapchainExtent.width = std::clamp(static_cast<uint32_t>(width),
                                  surfaceCaps.minImageExtent.width,
                                  surfaceCaps.maxImageExtent.width);
        m_swapchainExtent.height = std::clamp(static_cast<uint32_t>(height),
                                   surfaceCaps.minImageExtent.height,
                                   surfaceCaps.maxImageExtent.height);
    }



    const VkFormat imageFormat{VK_FORMAT_B8G8R8A8_SRGB };
    VkSwapchainCreateInfoKHR swapchainCI
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = window->getSurfaceHandle(),
        .minImageCount = surfaceCaps.minImageCount,
        .imageFormat = imageFormat,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = m_swapchainExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    if (vkCreateSwapchainKHR(cntx->getDeviceHandle(), &swapchainCI, nullptr, &m_VulkanSwapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Swapchain");
    }

    uint32_t imageCount{};
    vkGetSwapchainImagesKHR(cntx->getDeviceHandle(), m_VulkanSwapchain, &imageCount, nullptr);
    m_SwapchainImages.resize(imageCount);
    m_SwapchainImageViews.resize(imageCount);
    vkGetSwapchainImagesKHR(cntx->getDeviceHandle(), m_VulkanSwapchain, &imageCount, m_SwapchainImages.data());


    for (auto i = 0; i < imageCount; i++)
    {
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_SwapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = imageFormat,
            .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1},
        };

        if (vkCreateImageView(cntx->getDeviceHandle(), &imageViewCI, nullptr, &m_SwapchainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Swapchain Image Views");
        }
    }

}

VkSurfaceCapabilitiesKHR VulkanSwapchain::getSwapchainSurfaceCaps()
{
    VkSurfaceCapabilitiesKHR surfaceCaps{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(cntx->getPhysicalDeviceHandle(), window->getSurfaceHandle(), &surfaceCaps);

    return surfaceCaps;
}
