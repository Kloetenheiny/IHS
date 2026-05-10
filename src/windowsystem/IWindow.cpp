#include <windowsystem/IWindow.h>

#include "GLFW/glfw3.h"

IWindow::IWindow(VulkanContext* cntx)
    :cntx(cntx)
{
    createWindow();
    createSurface();
}

IWindow::~IWindow()
{
    windowCleanup();
}

void IWindow::createWindow()
{

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_Window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Vulkan Window", nullptr, nullptr);
    std::cout << "Created Window" << std::endl;

    int w, h;
    glfwGetFramebufferSize(m_Window, &w, &h);
    std::cout << "Framebuffer Size: " << w << "x" << h << std::endl;


}

void IWindow::getFramebufferSize(int *windowWidth, int *windwHeight)
{
    glfwGetFramebufferSize(m_Window, windowWidth, windwHeight);
}

void IWindow::createSurface()
{
    if (glfwCreateWindowSurface(cntx->getInstanceHandle(), m_Window, nullptr, &m_Surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Window Surface");
    }
}


void IWindow::windowCleanup()
{
    if (m_Surface)
    {
        vkDestroySurfaceKHR(cntx->getInstanceHandle(), m_Surface, nullptr);
    }


    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

}
