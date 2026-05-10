#include <renderer/vulkanbackend/VulkanContext.h>
#include <windowsystem/IWindow.h>
#include <renderer/vulkanbackend/VulkanSwapchain.h>
#include <renderer/vulkanbackend/VulkanAllocator.h>
#include <renderer/vulkanbackend/VulkanPipeline.h>


int main()
{
    glfwInit();

    VulkanContext Context{};
    IWindow window{&Context};
    VulkanSwapchain Swapchain{&Context, &window};
    VulkanAllocator Allocator{&Context, &Swapchain};
    VulkanGraphicsPipeline GraphicsPipeline{&Context, &Allocator};


    while (!glfwWindowShouldClose(window.getWindowHandle()))
    {
        glfwPollEvents();
    }


    return 0;
}