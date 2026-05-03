#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

int main() {
    // Application Info (optional, aber gute Praxis)
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Test";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Instance Info
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkInstance instance;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        std::cerr << "Fehler: Vulkan Instance konnte nicht erstellt werden\n";
        return -1;
    }

    std::cout << "Vulkan Instance erfolgreich erstellt!\n";

    // Verfügbare physische Geräte zählen
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    std::cout << "Gefundene GPUs mit Vulkan Support: " << deviceCount << "\n";

    if (deviceCount == 0) {
        std::cerr << "Keine Vulkan-kompatible GPU gefunden.\n";
    }

    // Aufräumen
    vkDestroyInstance(instance, nullptr);

    return 0;
}