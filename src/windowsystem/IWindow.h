#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

#include "renderer/vulkanbackend/VulkanContext.h"

class IWindow
{
private:
    //objects
    GLFWwindow* m_Window = nullptr;
    VulkanContext* cntx;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    int m_windowWidth = 1250;
    int m_windowHeight = 720;
public:
    //objects
private:
    //functions
    void createWindow();
    void createSurface();
    void windowCleanup();
public:
    //functions
    IWindow(VulkanContext* cntx);
    ~IWindow();
    int getwindowWidth() const {return m_windowWidth;}
    int getwindowHeight() const {return m_windowHeight;}
    void getFramebufferSize(int* windowWidth, int* windwHeight);
    GLFWwindow* getWindowHandle() const {return m_Window;}
    VkSurfaceKHR getSurfaceHandle() const {return m_Surface;}
};
