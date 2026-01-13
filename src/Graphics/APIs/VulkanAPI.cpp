#include "VulkanAPI.h"
#include "SDL/SDLManager.hpp"
#include "OpenXR/OpenXRManager.h"

#ifdef VULKAN_LINKED
VulkanAPI::VulkanAPI(SDLManager *sdlManager, OpenXRManager *openXRManager)
    : initialized_(false), openXRManager_(openXRManager), sdlManager_(sdlManager)
{
    instance_ = VK_NULL_HANDLE;
    physicalDevice_ = VK_NULL_HANDLE;
    logicalDevice_ = VK_NULL_HANDLE;

    graphicsQueueFamilyIndex_ = 0;
    computeQueueFamilyIndex_ = 0;
    SDLPresentQueueFamilyIndex_ = 0;

}

VulkanAPI::~VulkanAPI()
{
    cleanup();
}

bool VulkanAPI::fetchSDLInstanceExtensions()
{
    if (sdlManager_ == nullptr || !sdlManager_->isSDLValid())
    {
        logMessage(2, "Cannot fetch SDL instance extensions: SDL is not valid or available.", {"Graphics", "Vulkan", "SDL"});
        return false;
    }

    SDL_Window* window = sdlManager_->getWindow();
    if (window == nullptr)
    {
        logMessage(2, "Cannot fetch SDL instance extensions: SDL window is null.", {"Graphics", "Vulkan", "SDL"});
        return false;
    }

    uint32_t extensionCount = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr))
    {
        logMessage(2, "Failed to get SDL instance extension count.", {"Graphics", "Vulkan", "SDL"});
        return false;
    }

    sdlInstanceExtensions.resize(extensionCount);
    if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, sdlInstanceExtensions.data()))
    {
        logMessage(2, "Failed to get SDL instance extensions.", {"Graphics", "Vulkan", "SDL"});
        return false;
    }

    logMessage(3, "SDL instance extensions fetched successfully.", {"Graphics", "Vulkan", "SDL"});
    return true;
}

SDL_WindowFlags VulkanAPI::getSDLWindowFlags()
{
    return SDL_WINDOW_VULKAN;
}

bool VulkanAPI::printSDLInstanceExtensions()
{
    if (sdlInstanceExtensions.empty())
    {
        logMessage(4, "No SDL instance extensions to print.", {"Graphics", "Vulkan", "SDL"});
        return false;
    }

    std::ostringstream ss;
    ss << "SDL Instance Extensions for Vulkan (" << sdlInstanceExtensions.size() << " total):\n";
    for (const auto &ext : sdlInstanceExtensions)
    {
        ss << "  " << ext << "\n";
    }

    logMessage(4, ss.str(), {"Graphics", "Vulkan", "SDL"});
    return true;
}

bool VulkanAPI::fetchXRGraphicsRequirements()
{
    if (openXRManager_ == nullptr || !openXRManager_->isXRValid())
    {
        logMessage(2, "Cannot fetch XR graphics requirements: XR is not valid or available.", {"Graphics", "Vulkan", "OpenXR"});
        return false;
    }

    PFN_xrGetVulkanGraphicsRequirementsKHR pfnGetVulkanGraphicsRequirementsKHR = nullptr;
    XrResult pfnResult = xrGetInstanceProcAddr(*openXRManager_->getInstance(), "xrGetVulkanGraphicsRequirementsKHR",
                                               (PFN_xrVoidFunction *)&pfnGetVulkanGraphicsRequirementsKHR);
    if (pfnResult != XR_SUCCESS || pfnGetVulkanGraphicsRequirementsKHR == nullptr)
    {
        logMessage(2, "Failed to get xrGetVulkanGraphicsRequirementsKHR function pointer.", {"Graphics", "Vulkan", "OpenXR"});
        dirtyXRErrorPrint(pfnResult, *openXRManager_->getInstance());
        return false;
    }

    XrResult reqResult = pfnGetVulkanGraphicsRequirementsKHR(*openXRManager_->getInstance(), *openXRManager_->getSystemId(), &graphicsRequirements);
    if (reqResult != XR_SUCCESS)
    {
        logMessage(2, "Failed to get Vulkan graphics requirements.", {"Graphics", "Vulkan", "OpenXR"});
        dirtyXRErrorPrint(reqResult, *openXRManager_->getInstance());
        return false;
    }
    std::ostringstream ss;
    ss << "Vulkan graphics requirements retrieved successfully.\n"
       << "Min API version: " << XR_VERSION_MAJOR(graphicsRequirements.minApiVersionSupported) << "."
       << XR_VERSION_MINOR(graphicsRequirements.minApiVersionSupported) << "."
       << XR_VERSION_PATCH(graphicsRequirements.minApiVersionSupported) << "\n"
       << "Max API version: " << XR_VERSION_MAJOR(graphicsRequirements.maxApiVersionSupported) << "."
       << XR_VERSION_MINOR(graphicsRequirements.maxApiVersionSupported) << "."
       << XR_VERSION_PATCH(graphicsRequirements.maxApiVersionSupported);

    logMessage(4, ss.str(), {"Graphics", "Vulkan", "OpenXR"});
    return true;
}

bool VulkanAPI::fetchXRDeviceExtensions()
{
    if (openXRManager_ == nullptr || !openXRManager_->isXRValid())
    {
        logMessage(2, "Cannot fetch XR device extensions for Vulkan: XR is not valid or available.", {"Graphics", "Vulkan", "OpenXR"});
        return false;
    }

    PFN_xrGetVulkanDeviceExtensionsKHR pfnGetVulkanDeviceExtensionsKHR = nullptr;
    XrResult pfnResult = xrGetInstanceProcAddr(*openXRManager_->getInstance(), "xrGetVulkanDeviceExtensionsKHR",
                                               (PFN_xrVoidFunction *)&pfnGetVulkanDeviceExtensionsKHR);
    if (pfnResult != XR_SUCCESS || pfnGetVulkanDeviceExtensionsKHR == nullptr)
    {
        logMessage(2, "Failed to get xrGetVulkanDeviceExtensionsKHR function pointer.", {"Graphics", "Vulkan", "OpenXR"});
        dirtyXRErrorPrint(pfnResult, *openXRManager_->getInstance());
        return false;
    }

    uint32_t extensionNamesSize = 0;
    XrResult sizeResult = pfnGetVulkanDeviceExtensionsKHR(*openXRManager_->getInstance(), *openXRManager_->getSystemId(), 0, &extensionNamesSize, nullptr);
    if (sizeResult != XR_SUCCESS)
    {
        logMessage(2, "Failed to get XR device extensions size for Vulkan.", {"Graphics", "Vulkan", "OpenXR"});
        dirtyXRErrorPrint(sizeResult, *openXRManager_->getInstance());
        return false;
    }

    xrDeviceExtensionNames_.resize(extensionNamesSize);
    XrResult extResult = pfnGetVulkanDeviceExtensionsKHR(*openXRManager_->getInstance(), *openXRManager_->getSystemId(),
                                                         extensionNamesSize, &extensionNamesSize,
                                                         xrDeviceExtensionNames_.data());
    if (extResult != XR_SUCCESS)
    {
        logMessage(2, "Failed to get XR device extensions for Vulkan.", {"Graphics", "Vulkan", "OpenXR"});
        dirtyXRErrorPrint(extResult, *openXRManager_->getInstance());
        return false;
    }

    // Parse space-separated extension names into xrDeviceExtensions vector
    xrDeviceExtensions.clear();
    const char *ext = xrDeviceExtensionNames_.data();
    while (*ext != '\0')
    {
        xrDeviceExtensions.push_back(ext);
        while (*ext != ' ' && *ext != '\0')
            ext++;
        if (*ext == ' ')
        {
            const_cast<char *>(ext)[0] = '\0';
            ext++;
        }
    }

    logMessage(3, "XR device extensions for Vulkan fetched successfully.", {"Graphics", "Vulkan", "OpenXR"});
    return true;
}

bool VulkanAPI::printXRDeviceExtensions()
{
    if (xrDeviceExtensions.empty())
    {
        logMessage(4, "No XR device extensions for Vulkan to print.", {"Graphics", "Vulkan"});
        return false;
    }

    std::ostringstream ss;
    ss << "XR Device Extensions for Vulkan required by OpenXR (" << xrDeviceExtensions.size() << " total):\n";
    for (const auto &ext : xrDeviceExtensions)
    {
        ss << "  " << ext << "\n";
    }

    logMessage(4, ss.str(), {"Graphics", "Vulkan", "OpenXR"});
    return true;
}

bool VulkanAPI::fetchXRInstanceExtensions()
{
    if (openXRManager_ == nullptr || !openXRManager_->isXRValid())
    {
        logMessage(2, "Cannot fetch XR instance extensions for Vulkan: XR is not valid or available.", {"Graphics", "Vulkan", "OpenXR"});
        return false;
    }

    PFN_xrGetVulkanInstanceExtensionsKHR pfnGetVulkanInstanceExtensionsKHR = nullptr;
    XrResult pfnResult = xrGetInstanceProcAddr(*openXRManager_->getInstance(), "xrGetVulkanInstanceExtensionsKHR",
                                               (PFN_xrVoidFunction *)&pfnGetVulkanInstanceExtensionsKHR);
    if (pfnResult != XR_SUCCESS || pfnGetVulkanInstanceExtensionsKHR == nullptr)
    {
        logMessage(2, "Failed to get xrGetVulkanInstanceExtensionsKHR function pointer.", {"Graphics", "Vulkan", "OpenXR"});
        dirtyXRErrorPrint(pfnResult, *openXRManager_->getInstance());
        return false;
    }

    uint32_t extensionNamesSize = 0;
    XrResult sizeResult = pfnGetVulkanInstanceExtensionsKHR(*openXRManager_->getInstance(), *openXRManager_->getSystemId(), 0, &extensionNamesSize, nullptr);
    if (sizeResult != XR_SUCCESS)
    {
        logMessage(2, "Failed to get XR instance extensions size for Vulkan.", {"Graphics", "Vulkan", "OpenXR"});
        dirtyXRErrorPrint(sizeResult, *openXRManager_->getInstance());
        return false;
    }

    xrInstanceExtensionNames_.resize(extensionNamesSize);
    XrResult extResult = pfnGetVulkanInstanceExtensionsKHR(*openXRManager_->getInstance(), *openXRManager_->getSystemId(),
                                                           extensionNamesSize, &extensionNamesSize,
                                                           xrInstanceExtensionNames_.data());
    if (extResult != XR_SUCCESS)
    {
        logMessage(2, "Failed to get XR instance extensions for Vulkan.", {"Graphics", "Vulkan", "OpenXR"});
        dirtyXRErrorPrint(extResult, *openXRManager_->getInstance());
        return false;
    }

    // Parse space-separated extension names into xrInstanceExtensions vector
    xrInstanceExtensions.clear();
    const char *ext = xrInstanceExtensionNames_.data();
    while (*ext != '\0')
    {
        xrInstanceExtensions.push_back(ext);
        while (*ext != ' ' && *ext != '\0')
            ext++;
        if (*ext == ' ')
        {
            const_cast<char *>(ext)[0] = '\0';
            ext++;
        }
    }

    logMessage(3, "XR instance extensions for Vulkan fetched successfully.", {"Graphics", "Vulkan", "OpenXR"});
    return true;
}

bool VulkanAPI::printXRInstanceExtensions()
{
    if (xrInstanceExtensions.empty())
    {
        logMessage(4, "No XR instance extensions for Vulkan to print.", {"Graphics", "Vulkan"});
        return false;
    }

    std::ostringstream ss;
    ss << "XR Instance Extensions for Vulkan required by OpenXR (" << xrInstanceExtensions.size() << " total):\n";
    for (const auto &ext : xrInstanceExtensions)
    {
        ss << "  " << ext << "\n";
    }

    logMessage(4, ss.str(), {"Graphics", "Vulkan", "OpenXR"});
    return true;
}

bool VulkanAPI::fetchPhysicalDevices()
{
    if (instance_ == VK_NULL_HANDLE)
    {
        logMessage(2, "Cannot fetch physical devices: Vulkan instance not created.", {"Graphics", "Vulkan"});
        return false;
    }

    uint32_t deviceCount = 0;
    VkResult countResult = vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
    if (countResult != VK_SUCCESS || deviceCount == 0)
    {
        logMessage(2, "Failed to enumerate physical devices or no devices found.", {"Graphics", "Vulkan"});
        return false;
    }

    physicalDevices_.resize(deviceCount);
    VkResult enumResult = vkEnumeratePhysicalDevices(instance_, &deviceCount, physicalDevices_.data());
    if (enumResult != VK_SUCCESS)
    {
        logMessage(2, "Failed to enumerate physical devices.", {"Graphics", "Vulkan"});
        return false;
    }

    logMessage(3, "Physical devices fetched successfully. Count: " + std::to_string(deviceCount), {"Graphics", "Vulkan"});
    return true;
}

bool VulkanAPI::printPhysicalDevices()
{
    if (physicalDevices_.empty())
    {
        logMessage(4, "No physical devices to print.", {"Graphics", "Vulkan"});
        return false;
    }

    std::ostringstream ss;
    ss << "Available Vulkan Physical Devices (" << physicalDevices_.size() << " total):\n";

    for (size_t i = 0; i < physicalDevices_.size(); ++i)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physicalDevices_[i], &props);

        ss << "  [" << i << "] " << props.deviceName << "\n";
        ss << "      Device Type: ";
        switch (props.deviceType)
        {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            ss << "Discrete GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            ss << "Integrated GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            ss << "Virtual GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            ss << "CPU";
            break;
        default:
            ss << "Other";
            break;
        }
        ss << "\n";
        ss << "      API Version: " << VK_VERSION_MAJOR(props.apiVersion) << "."
           << VK_VERSION_MINOR(props.apiVersion) << "."
           << VK_VERSION_PATCH(props.apiVersion) << "\n";
        ss << "      Driver Version: " << props.driverVersion << "\n";
        ss << "      Vendor ID: 0x" << std::hex << props.vendorID << std::dec << "\n";
        ss << "      Device ID: 0x" << std::hex << props.deviceID << std::dec << "\n";
    }

    logMessage(4, ss.str(), {"Graphics", "Vulkan"});
    return true;
}

VkPhysicalDevice VulkanAPI::getOpenXRRecommendedDevice()
{
    if (openXRManager_ == nullptr || !openXRManager_->isXRValid())
    {
        logMessage(4, "OpenXR not valid or available, cannot get recommended device.", {"Graphics", "Vulkan", "OpenXR"});
        return VK_NULL_HANDLE;
    }

    if (instance_ == VK_NULL_HANDLE)
    {
        logMessage(4, "Vulkan instance not created, cannot get OpenXR recommended device.", {"Graphics", "Vulkan", "OpenXR"});
        return VK_NULL_HANDLE;
    }

    PFN_xrGetVulkanGraphicsDeviceKHR pfnGetVulkanGraphicsDeviceKHR = nullptr;
    XrResult pfnResult = xrGetInstanceProcAddr(*openXRManager_->getInstance(), "xrGetVulkanGraphicsDeviceKHR",
                                               (PFN_xrVoidFunction *)&pfnGetVulkanGraphicsDeviceKHR);
    if (pfnResult != XR_SUCCESS || pfnGetVulkanGraphicsDeviceKHR == nullptr)
    {
        logMessage(4, "Failed to get xrGetVulkanGraphicsDeviceKHR function pointer.", {"Graphics", "Vulkan", "OpenXR"});
        dirtyXRErrorPrint(pfnResult, *openXRManager_->getInstance());
        return VK_NULL_HANDLE;
    }

    VkPhysicalDevice recommendedDevice = VK_NULL_HANDLE;
    XrResult deviceResult = pfnGetVulkanGraphicsDeviceKHR(*openXRManager_->getInstance(), *openXRManager_->getSystemId(),
                                                          instance_, &recommendedDevice);
    if (deviceResult != XR_SUCCESS)
    {
        logMessage(4, "Failed to get OpenXR recommended Vulkan physical device.", {"Graphics", "Vulkan", "OpenXR"});
        dirtyXRErrorPrint(deviceResult, *openXRManager_->getInstance());
        return VK_NULL_HANDLE;
    }

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(recommendedDevice, &props);
    logMessage(3, "OpenXR recommended device: " + std::string(props.deviceName), {"Graphics", "Vulkan", "OpenXR"});

    return recommendedDevice;
}

bool VulkanAPI::selectPhysicalDevice()
{
    if (physicalDevices_.empty())
    {
        logMessage(2, "No physical devices available to select.", {"Graphics", "Vulkan"});
        return false;
    }

    // Try OpenXR recommendation first
    VkPhysicalDevice recommended = getOpenXRRecommendedDevice();
    if (recommended != VK_NULL_HANDLE)
    {
        // Verify the recommended device is in our list
        for (const auto &device : physicalDevices_)
        {
            if (device == recommended)
            {
                physicalDevice_ = recommended;
                VkPhysicalDeviceProperties props;
                vkGetPhysicalDeviceProperties(physicalDevice_, &props);
                logMessage(3, "Selected OpenXR recommended device: " + std::string(props.deviceName),
                           {"Graphics", "Vulkan"});
                return true;
            }
        }
    }

    // Fall back to first device
    physicalDevice_ = physicalDevices_[0];
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(physicalDevice_, &props);
    logMessage(3, "Selected first available device: " + std::string(props.deviceName),
               {"Graphics", "Vulkan"});
    return true;
}

bool VulkanAPI::fetchPhysicalDeviceQueueProperties()
{
    if (physicalDevice_ == VK_NULL_HANDLE)
    {
        logMessage(2, "Cannot fetch queue properties: No physical device selected.", {"Graphics", "Vulkan"});
        return false;
    }

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0)
    {
        logMessage(2, "No queue families found for the selected physical device.", {"Graphics", "Vulkan"});
        return false;
    }

    queueFamiliesP_.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, queueFamiliesP_.data());

    logMessage(3, "Fetched " + std::to_string(queueFamilyCount) + " queue families for the selected physical device.", {"Graphics", "Vulkan"});
    return true;
}

bool VulkanAPI::printPhysicalDeviceQueueProperties()
{
    if (queueFamiliesP_.empty())
    {
        logMessage(4, "No queue family properties to print.", {"Graphics", "Vulkan"});
        return false;
    }

    std::ostringstream ss;
    ss << "Queue Family Properties for Selected Vulkan Physical Device (" << queueFamiliesP_.size() << " total):\n";

    for (size_t i = 0; i < queueFamiliesP_.size(); ++i)
    {
        ss << "  [" << i << "] Queue Count: " << queueFamiliesP_[i].queueCount << "\n";
        ss << "      Queue Flags: ";
        if (queueFamiliesP_[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            ss << "GRAPHICS ";
        if (queueFamiliesP_[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            ss << "COMPUTE ";
        if (queueFamiliesP_[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
            ss << "TRANSFER ";
        if (queueFamiliesP_[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
            ss << "SPARSE_BINDING ";
        if (queueFamiliesP_[i].queueFlags & VK_QUEUE_PROTECTED_BIT)
            ss << "PROTECTED ";
        ss << "\n";
        ss << "      Timestamp Valid Bits: " << queueFamiliesP_[i].timestampValidBits << "\n";
        ss << "      Min Image Transfer Granularity: "
           << queueFamiliesP_[i].minImageTransferGranularity.width << "x"
           << queueFamiliesP_[i].minImageTransferGranularity.height << "x"
           << queueFamiliesP_[i].minImageTransferGranularity.depth << "\n";
    }

    logMessage(4, ss.str(), {"Graphics", "Vulkan"});
    return true;
}



bool VulkanAPI::findQueue(QueueType qtype, uint32_t* queueFamilyIndex, bool requireSurfaceSupport)
{
    if (queueFamiliesP_.empty())
    {
        logMessage(1, "No queue family properties available to find queue.", {"Graphics", "Vulkan"});
        return false;
    }
    if(qtype == QueueType::GRAPHICS)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamiliesP_.size()); ++i)
        {
            if ((queueFamiliesP_[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && queueFamiliesP_[i].queueCount > 0)
            {
                if (requireSurfaceSupport){
                    VkBool32 presentSupport = VK_FALSE;
                    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice_, i, SDLsurface_, &presentSupport);
                    if (!presentSupport) {
                        logMessage(1, "Queue family index " + std::to_string(i) + " does not support presentation to the SDL surface; skipping.", {"Graphics", "Vulkan"});
                        continue;
                    }
                }
                *queueFamilyIndex = i;
                return true;
            }
            {
                if (requireSurfaceSupport){
                    VkBool32 presentSupport = VK_FALSE;
                    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice_, i, SDLsurface_, &presentSupport);
                    if (!presentSupport) {
                        logMessage(1, "Queue family index " + std::to_string(i) + " does not support presentation to the SDL surface; skipping.", {"Graphics", "Vulkan"});
                        continue;
                    }
                }
                *queueFamilyIndex = i;
                return true;
            }
        }
    }
    else if(qtype == QueueType::COMPUTE)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamiliesP_.size()); ++i)
        {
            if (queueFamiliesP_[i].queueFlags & VK_QUEUE_COMPUTE_BIT && !(queueFamiliesP_[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                *queueFamilyIndex = i;
                return true;
            }
        }
        logMessage(3, "No dedicated compute queue found; falling back to any compute-capable queue.", {"Graphics", "Vulkan"});
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamiliesP_.size()); ++i)
        {
            if (queueFamiliesP_[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                *queueFamilyIndex = i;
                return true;
            }
        }
    }
    else if(qtype == QueueType::TRANSFER)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamiliesP_.size()); ++i)
        {
            if (queueFamiliesP_[i].queueFlags & VK_QUEUE_TRANSFER_BIT && !(queueFamiliesP_[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !(queueFamiliesP_[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
            {
                *queueFamilyIndex = i;
                return true;
            }
        }
        logMessage(3, "No dedicated transfer queue found; falling back to any transfer-capable queue.", {"Graphics", "Vulkan"});
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamiliesP_.size()); ++i)
        {
            if (queueFamiliesP_[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                *queueFamilyIndex = i;
                return true;
            }
        }
    }
    std::stringstream ss;

    logMessage(2, "No suitable queue family found for the requested type.", {"Graphics", "Vulkan"});
    return false;

}

bool VulkanAPI::createLogicalDevice()
{
    if (fetchPhysicalDeviceQueueProperties())
    {
        printPhysicalDeviceQueueProperties();
    }

    if (physicalDevice_ == VK_NULL_HANDLE)
    {
        logMessage(2, "Cannot create logical device: No physical device selected.", {"Graphics", "Vulkan"});
        return false;
    }

    bool requireSurfaceSupport = (sdlManager_ != nullptr && sdlManager_->isSDLValid() && sdlManager_->getWindow() != nullptr && SDLsurface_ != VK_NULL_HANDLE);
    // Select Graphics queue
    if (!findQueue(QueueType::GRAPHICS, &graphicsQueueFamilyIndex_))
    {
        logMessage(2, "Failed to find a suitable graphics queue, defaulting to 0.", {"Graphics", "Vulkan"});
        return false;
    }else{
        logMessage(3, "Selected graphics queue family index: " + std::to_string(graphicsQueueFamilyIndex_), {"Graphics", "Vulkan"});
    }
    // Select Compute queue
    if (!findQueue(QueueType::COMPUTE, &computeQueueFamilyIndex_))
    {
        logMessage(2, "Failed to find a suitable compute queue, defaulting to 0.", {"Graphics", "Vulkan"});
        return false;
    }else{
        logMessage(3, "Selected compute queue family index: " + std::to_string(computeQueueFamilyIndex_), {"Graphics", "Vulkan"});
    }
    // Select SDL Present queue
    if (requireSurfaceSupport)
    {
        if (!findQueue(QueueType::GRAPHICS, &SDLPresentQueueFamilyIndex_, true))
        {
            logMessage(2, "Failed to find a suitable SDL present queue, defaulting to graphics queue.", {"Graphics", "Vulkan"});
            SDLPresentQueueFamilyIndex_ = graphicsQueueFamilyIndex_;
        }else{
            logMessage(3, "Selected SDL present queue family index: " + std::to_string(SDLPresentQueueFamilyIndex_), {"Graphics", "Vulkan"});
        }
    }
    else
    {
        SDLPresentQueueFamilyIndex_ = graphicsQueueFamilyIndex_;
    }



    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;

    queueCreateInfos.push_back({.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                .queueFamilyIndex = graphicsQueueFamilyIndex_,
                                .queueCount = 1,
                                .pQueuePriorities = &queuePriority});

    queueCreateInfos.push_back({.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                .queueFamilyIndex = computeQueueFamilyIndex_,
                                .queueCount = 1,
                                .pQueuePriorities = &queuePriority});

    // Combine XR device extensions with swapchain extension if SDL is available
    std::vector<const char*> deviceExtensions = xrDeviceExtensions;
    if (requireSurfaceSupport)
    {
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    VkDeviceCreateInfo deviceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data()};

    VkResult deviceResult = vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &logicalDevice_);
    if (deviceResult != VK_SUCCESS)
    {
        std::stringstream ss;
        ss << "Failed to create logical device. VkResult: " << deviceResult;
        logMessage(2, ss.str(), {"Graphics", "Vulkan"});

        return false;
    }
    logMessage(3, "Logical device created successfully.", {"Graphics", "Vulkan"});

    return true;
}

bool VulkanAPI::printLogicalDeviceFeatures()
{
    if (physicalDevice_ == VK_NULL_HANDLE)
    {
        logMessage(2, "Cannot print logical device features: No physical device selected.", {"Graphics", "Vulkan"});
        return false;
    }

    VkPhysicalDeviceFeatures features{};
    vkGetPhysicalDeviceFeatures(physicalDevice_, &features);

    auto boolStr = [](VkBool32 v)
    { return v ? "True" : "False"; };

    std::ostringstream ss;
    ss << "Logical Device Features for Selected Vulkan Physical Device:\n";
    ss << "  robustBufferAccess: " << boolStr(features.robustBufferAccess) << "\n";
    ss << "  fullDrawIndexUint32: " << boolStr(features.fullDrawIndexUint32) << "\n";
    ss << "  imageCubeArray: " << boolStr(features.imageCubeArray) << "\n";
    ss << "  independentBlend: " << boolStr(features.independentBlend) << "\n";
    ss << "  geometryShader: " << boolStr(features.geometryShader) << "\n";
    ss << "  tessellationShader: " << boolStr(features.tessellationShader) << "\n";
    ss << "  sampleRateShading: " << boolStr(features.sampleRateShading) << "\n";
    ss << "  dualSrcBlend: " << boolStr(features.dualSrcBlend) << "\n";
    ss << "  logicOp: " << boolStr(features.logicOp) << "\n";
    ss << "  multiDrawIndirect: " << boolStr(features.multiDrawIndirect) << "\n";
    ss << "  drawIndirectFirstInstance: " << boolStr(features.drawIndirectFirstInstance) << "\n";
    ss << "  depthClamp: " << boolStr(features.depthClamp) << "\n";
    ss << "  depthBiasClamp: " << boolStr(features.depthBiasClamp) << "\n";
    ss << "  fillModeNonSolid: " << boolStr(features.fillModeNonSolid) << "\n";
    ss << "  depthBounds: " << boolStr(features.depthBounds) << "\n";
    ss << "  wideLines: " << boolStr(features.wideLines) << "\n";
    ss << "  largePoints: " << boolStr(features.largePoints) << "\n";
    ss << "  alphaToOne: " << boolStr(features.alphaToOne) << "\n";
    ss << "  multiViewport: " << boolStr(features.multiViewport) << "\n";
    ss << "  samplerAnisotropy: " << boolStr(features.samplerAnisotropy) << "\n";
    ss << "  textureCompressionETC2: " << boolStr(features.textureCompressionETC2) << "\n";
    ss << "  textureCompressionASTC_LDR: " << boolStr(features.textureCompressionASTC_LDR) << "\n";
    ss << "  textureCompressionBC: " << boolStr(features.textureCompressionBC) << "\n";
    ss << "  occlusionQueryPrecise: " << boolStr(features.occlusionQueryPrecise) << "\n";
    ss << "  pipelineStatisticsQuery: " << boolStr(features.pipelineStatisticsQuery) << "\n";
    ss << "  vertexPipelineStoresAndAtomics: " << boolStr(features.vertexPipelineStoresAndAtomics) << "\n";
    ss << "  fragmentStoresAndAtomics: " << boolStr(features.fragmentStoresAndAtomics) << "\n";
    ss << "  shaderImageGatherExtended: " << boolStr(features.shaderImageGatherExtended) << "\n";
    ss << "  shaderStorageImageExtendedFormats: " << boolStr(features.shaderStorageImageExtendedFormats) << "\n";
    ss << "  shaderStorageImageMultisample: " << boolStr(features.shaderStorageImageMultisample) << "\n";
    ss << "  shaderStorageImageReadWithoutFormat: " << boolStr(features.shaderStorageImageReadWithoutFormat) << "\n";
    ss << "  shaderStorageImageWriteWithoutFormat: " << boolStr(features.shaderStorageImageWriteWithoutFormat) << "\n";
    ss << "  shaderUniformBufferArrayDynamicIndexing: " << boolStr(features.shaderUniformBufferArrayDynamicIndexing) << "\n";
    ss << "  shaderSampledImageArrayDynamicIndexing: " << boolStr(features.shaderSampledImageArrayDynamicIndexing) << "\n";
    ss << "  shaderStorageBufferArrayDynamicIndexing: " << boolStr(features.shaderStorageBufferArrayDynamicIndexing) << "\n";
    ss << "  shaderStorageImageArrayDynamicIndexing: " << boolStr(features.shaderStorageImageArrayDynamicIndexing) << "\n";
    ss << "  shaderClipDistance: " << boolStr(features.shaderClipDistance) << "\n";
    ss << "  shaderCullDistance: " << boolStr(features.shaderCullDistance) << "\n";
    ss << "  shaderFloat64: " << boolStr(features.shaderFloat64) << "\n";
    ss << "  shaderInt64: " << boolStr(features.shaderInt64) << "\n";
    ss << "  shaderInt16: " << boolStr(features.shaderInt16) << "\n";
    ss << "  variableMultisampleRate: " << boolStr(features.variableMultisampleRate) << "\n";
    ss << "  inheritedQueries: " << boolStr(features.inheritedQueries) << "\n";

    logMessage(4, ss.str(), {"Graphics", "Vulkan"});
    return true;
}

bool VulkanAPI::fetchQueues(){
    if (logicalDevice_ == VK_NULL_HANDLE)
    {
        logMessage(2, "Cannot fetch queues: Logical device not created.", {"Graphics", "Vulkan"});
        return false;
    }

    vkGetDeviceQueue(logicalDevice_, graphicsQueueFamilyIndex_, 0, &graphicsQueue_);
    vkGetDeviceQueue(logicalDevice_, computeQueueFamilyIndex_, 0, &computeQueue_);
    if( sdlManager_ != nullptr && sdlManager_->isSDLValid())
        vkGetDeviceQueue(logicalDevice_, SDLPresentQueueFamilyIndex_, 0, &SDLPresentQueue_);

    logMessage(3, "Fetched graphics, compute, and SDL present queues from logical device.", {"Graphics", "Vulkan"});
    return true;
}

bool VulkanAPI::getGraphicsBinding(XrGraphicsBindingVulkanKHR &graphicsBinding)
{
    // Loads the Vulkan graphics binding structure for OpenXR
    // TODO: Add Implementation for vulkan openxr graphics binding
    return false;
}

bool VulkanAPI::createSDLSurface()
{
    if (sdlManager_ == nullptr)
    {
        logMessage(2, "Cannot create SDL surface: SDLManager is null.", {"Graphics", "Vulkan", "SDL"});
        return false;
    }

    SDL_Window* window = sdlManager_->getWindow();
    if (window == nullptr)
    {
        logMessage(2, "Cannot create SDL surface: SDL window is null.", {"Graphics", "Vulkan", "SDL"});
        return false;
    }

    if (!SDL_Vulkan_CreateSurface(window, instance_, &SDLsurface_))
    {
        logMessage(2, "Failed to create Vulkan surface from SDL window.", {"Graphics", "Vulkan", "SDL"});
        return false;
    }

    logMessage(3, "SDL Vulkan surface created successfully.", {"Graphics", "Vulkan", "SDL"});

    std::stringstream ss;
    ss << "SDL surface creation errors: " << SDL_GetError();
    logMessage(4, ss.str(), {"Graphics", "Vulkan", "SDL"});
    return true;
}

bool VulkanAPI::createSwapchain(){
    if (SDLsurface_ == VK_NULL_HANDLE)
    {
        logMessage(2, "Cannot create swapchain: SDL surface is null.", {"Graphics", "Vulkan"});
        return false;
    }

    if (physicalDevice_ == VK_NULL_HANDLE)
    {
        logMessage(2, "Cannot create swapchain: Physical device is null.", {"Graphics", "Vulkan"});
        return false;
    }

    // Query surface capabilities
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VkResult capResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice_, SDLsurface_, &surfaceCapabilities);
    if (capResult != VK_SUCCESS)
    {
        std::stringstream ss;
        ss << "Failed to get surface capabilities. VkResult: " << capResult;
        logMessage(2, ss.str(), {"Graphics", "Vulkan"});
        return false;
    }

    // Query surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, SDLsurface_, &formatCount, nullptr);
    if (formatCount == 0)
    {
        logMessage(2, "No surface formats available.", {"Graphics", "Vulkan"});
        return false;
    }
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, SDLsurface_, &formatCount, surfaceFormats.data());

    // Choose surface format
    VkSurfaceFormatKHR chosenFormat = surfaceFormats[0];
    for (const auto& format : surfaceFormats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            chosenFormat = format;
            break;
        }
    }

    // Query present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice_, SDLsurface_, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice_, SDLsurface_, &presentModeCount, presentModes.data());

    // Choose present mode (prefer FIFO as it's always available)
    VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Determine extent
    VkExtent2D extent;
    if (surfaceCapabilities.currentExtent.width != UINT32_MAX)
    {
        extent = surfaceCapabilities.currentExtent;
    }
    else
    {
        int width = 1280;
        int height = 720;
        sdlManager_->getResolution(width, height);
        
        extent.width = std::max(surfaceCapabilities.minImageExtent.width,
                               std::min(surfaceCapabilities.maxImageExtent.width, static_cast<uint32_t>(width)));
        extent.height = std::max(surfaceCapabilities.minImageExtent.height,
                                std::min(surfaceCapabilities.maxImageExtent.height, static_cast<uint32_t>(height)));
    }

    // Determine image count
    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        nullptr,
        0,
        SDLsurface_,
        imageCount,
        chosenFormat.format,
        chosenFormat.colorSpace,
        extent,
        1,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        nullptr,
        surfaceCapabilities.currentTransform,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        chosenPresentMode,
        VK_TRUE,
        VK_NULL_HANDLE};

    VkResult swapchainResult = vkCreateSwapchainKHR(logicalDevice_, &swapchainCreateInfo, nullptr, &swapchain_);
    if (swapchainResult != VK_SUCCESS)
    {
        std::stringstream ss;
        ss << "Failed to create Vulkan swapchain. VkResult: " << swapchainResult;
        logMessage(1, ss.str(), {"Graphics", "Vulkan"});
        return false;
    }
    logMessage(3, "Vulkan swapchain created successfully.", {"Graphics", "Vulkan"});
    return true;
}

bool VulkanAPI::generateSwapchainImageViews(){
    if (swapchain_ == VK_NULL_HANDLE)
    {
        logMessage(2, "Cannot generate swapchain image views: Swapchain is null.", {"Graphics", "Vulkan"});
        return false;
    }

    uint32_t imageCount = 0;
    VkResult countResult = vkGetSwapchainImagesKHR(logicalDevice_, swapchain_, &imageCount, nullptr);
    if (countResult != VK_SUCCESS || imageCount == 0)
    {
        logMessage(2, "Failed to get swapchain image count or no images available.", {"Graphics", "Vulkan"});
        return false;
    }

    swapchainImages_.resize(imageCount);
    VkResult enumResult = vkGetSwapchainImagesKHR(logicalDevice_, swapchain_, &imageCount, swapchainImages_.data());
    if (enumResult != VK_SUCCESS)
    {
        logMessage(2, "Failed to get swapchain images.", {"Graphics", "Vulkan"});
        return false;
    }

    swapchainImageViews_.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; ++i)
    {
        VkImageViewCreateInfo viewCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchainImages_[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_B8G8R8A8_SRGB,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1}};

        VkResult viewResult = vkCreateImageView(logicalDevice_, &viewCreateInfo, nullptr, &swapchainImageViews_[i]);
        if (viewResult != VK_SUCCESS)
        {
            std::stringstream ss;
            ss << "Failed to create image view for swapchain image " << i << ". VkResult: " << viewResult;
            logMessage(2, ss.str(), {"Graphics", "Vulkan"});
            return false;
        }
    }
    logMessage(3, "Swapchain image views created successfully.", {"Graphics", "Vulkan"});

    return true;
}

bool VulkanAPI::initAPI()
{
    logMessage(3, "Initializing Vulkan API...", {"Graphics", "Vulkan"});

    if (openXRManager_ == nullptr || !openXRManager_->isXRValid())
    {
        logMessage(2, "OpenXR not available or valid, proceeding without OpenXR integration.", {"Graphics", "Vulkan", "OpenXR"});
    }
    else
    {

        if (!fetchXRGraphicsRequirements())
        {
            logMessage(2, "Defaulting to minimum Vulkan API version.", {"Graphics", "Vulkan", "OpenXR"});
            graphicsRequirements.minApiVersionSupported = VK_API_VERSION_1_0; // Or query vkEnumerateInstanceVersion if instance exists
        }

        if (!fetchXRInstanceExtensions())
        {
            logMessage(2, "Failed to fetch XR instance extensions for Vulkan. Proceeding without XR extensions.", {"Graphics", "Vulkan", "OpenXR"});
        }
        else
        {
            printXRInstanceExtensions();
        }

        if (!fetchXRDeviceExtensions())
        {
            logMessage(2, "Failed to fetch XR device extensions for Vulkan. Proceeding without XR device extensions.", {"Graphics", "Vulkan", "OpenXR"});
        }
        else
        {
            printXRDeviceExtensions();
        }
    }
    
    if (sdlManager_ == nullptr || !sdlManager_->isSDLValid())
    {
        logMessage(2, "SDL not available or valid, proceeding without SDL integration.", {"Graphics", "Vulkan", "SDL"});
    }
    else
    {
        if (!fetchSDLInstanceExtensions())
        {
            logMessage(2, "Failed to fetch SDL instance extensions for Vulkan. Proceeding without SDL extensions.", {"Graphics", "Vulkan", "SDL"});
        }
        else
        {
            printSDLInstanceExtensions();
        }
    }

    // combine extensions
    std::vector<const char *> allExtensions;
    allExtensions.insert(allExtensions.end(), sdlInstanceExtensions.begin(), sdlInstanceExtensions.end());
    allExtensions.insert(allExtensions.end(), xrInstanceExtensions.begin(), xrInstanceExtensions.end());

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VRTestProj";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = graphicsRequirements.minApiVersionSupported;

    VkInstanceCreateInfo vkCreateInfo{};
    vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkCreateInfo.pApplicationInfo = &appInfo;
    vkCreateInfo.enabledExtensionCount = static_cast<uint32_t>(allExtensions.size());
    vkCreateInfo.ppEnabledExtensionNames = allExtensions.data();

    VkResult vk_instance_result = vkCreateInstance(&vkCreateInfo, nullptr, &instance_);
    if (vk_instance_result != VK_SUCCESS)
    {
        logMessage(1, "Failed to create Vulkan instance.", {"Graphics", "Vulkan"});
        return false;
    }

    // Fetch and select physical device
    if (!fetchPhysicalDevices())
    {
        logMessage(1, "Failed to fetch physical devices.", {"Graphics", "Vulkan"});
        cleanup();
        return false;
    }

    printPhysicalDevices();

    if (!selectPhysicalDevice())
    {
        logMessage(1, "Failed to select physical device.", {"Graphics", "Vulkan"});
        cleanup();
        return false;
    }

    // TODO : Create SDL Surface if SDL is available
    if(!createSDLSurface())
    {
        logMessage(2, "Failed to create SDL Vulkan surface. Proceeding without SDL surface.", {"Graphics", "Vulkan", "SDL"});
    }

    if (!createLogicalDevice())
    {
        logMessage(1, "Failed to create logical device.", {"Graphics", "Vulkan"});
        cleanup();
        return false;
    }
    printLogicalDeviceFeatures();

    if (!fetchQueues())
    {
        logMessage(1, "Failed to fetch queues from logical device.", {"Graphics", "Vulkan"});
        cleanup();
        return false;
    }

    if (!createSwapchain()){
        logMessage(1, "Failed to create swapchain.", {"Graphics", "Vulkan"});
        cleanup();
        return false;
    }

    if (!generateSwapchainImageViews()){
        logMessage(1, "Failed to create swapchain image views.", {"Graphics", "Vulkan"});
        cleanup();
        return false;
    }




    logMessage(3, "Vulkan API initialized successfully.", {"Graphics", "Vulkan"});
    initialized_ = true;
    return true;
}

Version VulkanAPI::getVersion()
{
    uint32_t apiVersion = 0;
    vkEnumerateInstanceVersion(&apiVersion);

    return Version{
        VK_VERSION_MAJOR(apiVersion),
        VK_VERSION_MINOR(apiVersion),
        VK_VERSION_PATCH(apiVersion)};
}

bool VulkanAPI::cleanup()
{
    if (!initialized_)
    {
        return true;
    }

    logMessage(3, "Cleaning up Vulkan API...", {"Graphics", "Vulkan"});

    if (logicalDevice_ != VK_NULL_HANDLE)
    {
        vkDestroyDevice(logicalDevice_, nullptr);
        logicalDevice_ = VK_NULL_HANDLE;
    }

    if (instance_ != VK_NULL_HANDLE)
    {
        vkDestroyInstance(instance_, nullptr);
        instance_ = VK_NULL_HANDLE;
    }

    // Clear extension vectors
    sdlInstanceExtensions.clear();
    xrInstanceExtensions.clear();
    xrInstanceExtensionNames_.clear();
    xrDeviceExtensions.clear();
    xrDeviceExtensionNames_.clear();
    physicalDevices_.clear();
    physicalDevice_ = VK_NULL_HANDLE;

    initialized_ = false;
    return true;
}

#endif // VULKAN_LINKED
