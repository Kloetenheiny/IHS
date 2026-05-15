#define VMA_IMPLEMENTATION
#include <renderer/vulkanbackend/VulkanContext.h>


VulkanContext::VulkanContext()
{
    createInstance();
    createDevice();
    createAllocator();
    CheckValidationLayerSupport();
}

VulkanContext::~VulkanContext()
{
    if (m_allocator_instance != VK_NULL_HANDLE)
    {
        vmaDestroyAllocator(m_allocator_instance);
        std::cout << "Destroyed VMA" << std::endl;
    }
    vkDestroyDevice(m_VulkanDevice, nullptr);
    vkDestroyInstance(m_VulkanInstance, nullptr);
}

void VulkanContext::createInstance()
{
    VkApplicationInfo appInfo
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "IHS",
        .applicationVersion = 1,
        .pEngineName = "IHS",
        .apiVersion = VK_API_VERSION_1_3,
    };

    uint32_t instanceExtCount{};
    char const* const* instExtensions = glfwGetRequiredInstanceExtensions(&instanceExtCount);


    VkInstanceCreateInfo instCI
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(m_validationLayer.size()),
        .ppEnabledLayerNames = m_validationLayer.data(),
        .enabledExtensionCount = instanceExtCount,
        .ppEnabledExtensionNames = instExtensions,

    };

    VkResult result = vkCreateInstance(&instCI, nullptr, &m_VulkanInstance);
    std::cerr << "vkCreateInstance result: " << result << std::endl;
    if (result != VK_SUCCESS)
        throw std::runtime_error("Failed to create Instance");
}

void VulkanContext::createDevice()
{

    uint32_t deviceCount{};
    vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, physicalDevices.data());

    VkPhysicalDevice integratedGPU = VK_NULL_HANDLE;

    for (const auto& device : physicalDevices)
    {
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(device, &props);

        std::cout << "Found GPU: " << props.deviceName << std::endl;

        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            m_VulkanPhysicalDevice = device;

            std::cout << "Selected discrete GPU: "
                      << props.deviceName << std::endl;
            std::cout << "Max push constant size: " << props.limits.maxPushConstantsSize << std::endl;

            break;
        }

        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            integratedGPU = device;

            std::cout << "Found integrated GPU: "
                      << props.deviceName << std::endl;
        }
    }

    if (m_VulkanPhysicalDevice == VK_NULL_HANDLE)
    {
        m_VulkanPhysicalDevice = integratedGPU;

        if (integratedGPU != VK_NULL_HANDLE)
        {
            std::cout << "Falling back to integrated GPU"
                      << std::endl;
        }
    }

    if (m_VulkanPhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("No suitable GPU found");
    }

    const float qfPriorities{1.0f};
    m_graphicsQueueFamilyIndex = getGraphicsQueueFamilyIndex();
    VkDeviceQueueCreateInfo queueCI
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = m_graphicsQueueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &qfPriorities
    };

    VkPhysicalDeviceVulkan12Features enabledVk12Features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .descriptorIndexing = true,
        .descriptorBindingVariableDescriptorCount = true,
        .runtimeDescriptorArray = true,
        .bufferDeviceAddress = true
    };
    const VkPhysicalDeviceVulkan13Features enabledVk13Features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &enabledVk12Features,
        .synchronization2 = true,
        .dynamicRendering = true,
    };
    const VkPhysicalDeviceFeatures enabledVk10Features{
        .samplerAnisotropy = VK_TRUE
    };

    VkDeviceCreateInfo deviceCI
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &enabledVk13Features,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCI,
        .enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size()),
        .ppEnabledExtensionNames = m_deviceExtensions.data(),
        .pEnabledFeatures = &enabledVk10Features,
    };


    if (vkCreateDevice(m_VulkanPhysicalDevice, &deviceCI, nullptr, &m_VulkanDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Device");
    }

    //get handle to graphics queue
    vkGetDeviceQueue(m_VulkanDevice, m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue);

}

uint32_t VulkanContext::findGraphicsQueueFamilyIndex()
{
    uint32_t queueCount{};
    vkGetPhysicalDeviceQueueFamilyProperties(m_VulkanPhysicalDevice, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> QueueFamProps(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_VulkanPhysicalDevice, &queueCount, QueueFamProps.data());

    uint32_t graphicsQueueIndex{};

    for (const auto& queueID : QueueFamProps)
    {
        if (queueID.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            return graphicsQueueIndex;
        }

        graphicsQueueIndex++;
    }

    throw std::runtime_error("No Graphics Queue Index found");
}

void VulkanContext::createAllocator()
{
    VmaVulkanFunctions vkFunctions
    {
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        .vkCreateImage = vkCreateImage
    };

    VmaAllocatorCreateInfo allocatorCI
    {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = m_VulkanPhysicalDevice,
        .device = m_VulkanDevice,
        .pVulkanFunctions = &vkFunctions,
        .instance = m_VulkanInstance,
        .vulkanApiVersion = VK_API_VERSION_1_3,

    };

    if (vmaCreateAllocator(&allocatorCI, &m_allocator_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Memory Allocator");
    }
}

bool VulkanContext::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount,availableLayers.data());

    for (const auto* layerName : m_validationLayer)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}
