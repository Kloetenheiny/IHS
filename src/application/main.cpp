#include <renderer/vulkanbackend/VulkanRenderer.h>



int main()
{
    glfwInit();

    VulkanRenderer Renderer{};

    Renderer.draw();

    return 0;
}