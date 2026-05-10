cmake_minimum_required(VERSION 3.16)
project(IHS)

set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS      OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS        OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS       OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES    OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL           OFF CACHE BOOL "" FORCE)

add_subdirectory(thirdparty/glfw)

find_package(Vulkan REQUIRED)

add_executable(IHS src/application/main.cpp
        src/renderer/vulkanbackend/VulkanContext.cpp
        src/renderer/vulkanbackend/VulkanContext.h
        src/renderer/vulkanbackend/VulkanAllocator.cpp
        src/renderer/vulkanbackend/VulkanAllocator.h
        src/windowsystem/IWindow.cpp
        src/windowsystem/IWindow.h
        src/renderer/vulkanbackend/VulkanSwapchain.cpp
        src/renderer/vulkanbackend/VulkanSwapchain.h
        src/renderer/vulkanbackend/VulkanRenderer.cpp
        src/renderer/vulkanbackend/VulkanRenderer.h
        src/renderer/vulkanbackend/VulkanPipeline.h
        src/renderer/vulkanbackend/VulkanPipeline.cpp)

target_include_directories(IHS PRIVATE
    src/
    thirdparty/
    thirdparty/glm
    thirdparty/vma/include
)

target_link_libraries(IHS PRIVATE
        Vulkan::Vulkan
        glfw

)