#pragma once
#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanContext.h"
#include "windowsystem/IWindow.h"

class VulkanSwapchain
{
private:
    //objects

    std::vector<VkImage> m_SwapchainImages;
    std::vector<VkImageView> m_SwapchainImageViews;
    VulkanContext* cntx;
    IWindow* window;
public:
    //objects
    VkExtent2D m_swapchainExtent{};
    VkSwapchainKHR m_VulkanSwapchain = VK_NULL_HANDLE;
private:
    //functions
    void createSwapchain();
    VkSurfaceCapabilitiesKHR getSwapchainSurfaceCaps();

public:
    //functions
    VulkanSwapchain(VulkanContext* cntx, IWindow* window);
    ~VulkanSwapchain();
    VkSwapchainKHR getSwapchainHandle() const {return m_VulkanSwapchain;}
    const std::vector<VkImage>& getSwapchainImages() const {return m_SwapchainImages;}
    const std::vector<VkImageView>& getSwapchainImageViews() const {return m_SwapchainImageViews;}

};
