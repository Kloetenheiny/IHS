cmake_minimum_required(VERSION 3.16)
project(VulkanTest)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_executable(VulkanTest main.cpp)

target_link_libraries(VulkanTest PRIVATE Vulkan::Vulkan)