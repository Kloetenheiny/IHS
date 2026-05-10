#include <renderer/vulkanbackend/VulkanContext.h>
#include <windowsystem/IWindow.h>
#include <renderer/vulkanbackend/VulkanSwapchain.h>
#include <renderer/vulkanbackend/VulkanAllocator.h>


int main()
{
    glfwInit();

    VulkanContext Context{};
    IWindow window{&Context};
    VulkanSwapchain Swapchain{&Context, &window};
    VulkanAllocator Allocator{&Context};

    while (!glfwWindowShouldClose(window.getWindowHandle()))
    {
        glfwPollEvents();
    }


    return 0;
}