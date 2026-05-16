#include <renderer/vulkanbackend/VulkanRenderer.h>



int main()
{
    glfwInit();

    VulkanRenderer Renderer{};

    Renderer.draw();

    std::cerr << sizeof(Renderer) << std::endl;

    return 0;
}