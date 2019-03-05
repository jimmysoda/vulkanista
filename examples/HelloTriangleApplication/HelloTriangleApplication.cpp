#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>

VkResult createDebugUtilsMessengerEXT(VkInstance &instance,
    const VkDebugUtilsMessengerCreateInfoEXT* info,
    const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT* messenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    return func != nullptr ? func(instance, info, allocator, messenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroyDebugUtilsMessengerEXT(VkInstance &instance,
    VkDebugUtilsMessengerEXT &messenger,
    const VkAllocationCallbacks *allocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr)
    {
        func(instance, messenger, allocator);
    }
}

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value();
    }
};

class HelloTriangleApplication
{
public:
    void run();

private:
    /**
     * Handles a Vulkan debug callback message.
     * 
     * @param severity The message's severity, i.e., verbose, info, warning, or error
     * @param type The message's type, i.e., general, validation, or performance
     * @param callbackData The details of the message, i.e., message text and Vulkan objects
     * @param userData User defined data
     */
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
        void *userData
    )
    {
        std::cerr << "validation layer: " << callbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    void initWindow();
    void initVulkan();
    std::vector<const char *> getRequiredExtensions() const;
    void createInstance();
    void setupDebugMessenger();
    bool checkValidationLayerSupport();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void mainLoop();
    void cleanup();

private:
    const int WIDTH = 800;
    const int HEIGHT = 600;

    const std::vector<const char *> mValidationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
    };

#ifdef NDEBUG
    const bool mEnableValidationLayers = false;
#else
    const bool mEnableValidationLayers = true;
#endif

    VkInstance mInstance;
    VkDebugUtilsMessengerEXT mDebugMessenger;
    VkDevice mDevice;
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkQueue mGraphicsQueue;
    GLFWwindow *mWindow;
};

/* static */
QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

        // Find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT
    for (uint32_t i = 0; i < count && !indices.isComplete(); ++i)
    {
        if (families[i].queueCount > 0 && families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }
    }

    return indices;
}

void HelloTriangleApplication::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void HelloTriangleApplication::initWindow()
{
    glfwInit();

    // Initialize glfw without creating an OpenGL context (i.e., GLFW_NO_API option)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    mWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void HelloTriangleApplication::initVulkan()
{
    createInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
    createLogicalDevice();
}

std::vector<const char *> HelloTriangleApplication::getRequiredExtensions() const
{
    // glfw extensions are always required
    uint32_t count = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + count);

    if (mEnableValidationLayers)
    {
        // Add debug messenger extension if validation layers are enabled
        // The VK_EXT_DEBUG_UTILS_EXTENSION_NAME macro is equal to
        // the literal string "VK_EXT_debug_utils".
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void HelloTriangleApplication::createInstance()
{
    if (mEnableValidationLayers && !checkValidationLayerSupport())
    {
        throw std::runtime_error("Vulkan validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo = {};
    // The default initialization sets pNext to nullptr
    // Many Vulkan structs expect the type to be specified explicitly in sType
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (mEnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
        createInfo.ppEnabledLayerNames = mValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan instance");
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> propertyList(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, propertyList.data());

    std::cout << "Available Vulkan extensions:" << std::endl;

    for (const auto &properties : propertyList)
    {
        std::cout << "\t" << properties.extensionName << std::endl;
    }
}

void HelloTriangleApplication::setupDebugMessenger()
{
    if (mEnableValidationLayers)
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;

        // Create the debug messenger with the default allocator
        const VkAllocationCallbacks *allocator = nullptr;
 
        if (createDebugUtilsMessengerEXT(mInstance, &createInfo, allocator, &mDebugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to set up debug messenger!");
        }
    }
}

void HelloTriangleApplication::pickPhysicalDevice()
{
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(mInstance, &count, nullptr);

    if (count == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(mInstance, &count, devices.data());

    auto isDeviceSuitable = [](VkPhysicalDevice device) {
        return findQueueFamilies(device).isComplete();
    };

    for (const auto &device : devices)
    {
        if (isDeviceSuitable(device))
        {
            mPhysicalDevice = device;
            break;
        }
    }

    if (mPhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(mPhysicalDevice, &properties);
    std::cout << "Physical device: " << properties.deviceName << std::endl;
}

void HelloTriangleApplication::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if (mEnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
        createInfo.ppEnabledLayerNames = mValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    // Assign the graphics queue
    vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
}

bool HelloTriangleApplication::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool ok = true;

    for (const char *layer : mValidationLayers)
    {
        bool found = false;

        for (auto &itr = availableLayers.cbegin(); !found && itr != availableLayers.cend(); ++itr)
        {
            if (std::strcmp(layer, itr->layerName) == 0)
            {
                found = true;
            }
        }

        if (!found)
        {
            ok = false;
            break;
        }
    }

    return ok;
}

void HelloTriangleApplication::mainLoop()
{
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
    }
}

void HelloTriangleApplication::cleanup()
{
    vkDestroyDevice(mDevice, nullptr);

    if (mEnableValidationLayers)
    {
        destroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
    }

    vkDestroyInstance(mInstance, nullptr);
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

int main()
{
    HelloTriangleApplication app;

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}