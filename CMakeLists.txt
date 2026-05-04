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
        src/renderer/vulkanbackend/VulkanContext.h)

target_include_directories(IHS PRIVATE
    src/
    thirdparty/
)

target_link_libraries(IHS PRIVATE
        Vulkan::Vulkan
        glfw

)