#include <renderer/vulkanbackend/VulkanContext.h>
#include <windowsystem/IWindow.h>
#include <renderer/vulkanbackend/VulkanSwapchain.h>


int main()
{
    glfwInit();

    VulkanContext Context{};
    IWindow window{&Context};
    VulkanSwapchain Swapchain{&Context, &window};

    while (!glfwWindowShouldClose(window.getWindowHandle()))
    {
        glfwPollEvents();
    }


    return 0;
}