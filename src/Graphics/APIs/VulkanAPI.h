#ifndef VULKANAPI_H
#define VULKANAPI_H

#ifdef VULKAN_LINKED

#include "GraphicsAPI.h"

#include "Utils/Utils.hpp"
#include "OpenXR/XRUtils.hpp"

#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>

class SDLManager; // Forward declaration
class OpenXRManager; // Forward declaration

enum class QueueType
{
    GRAPHICS,
    COMPUTE,
    TRANSFER
};



class VulkanAPI : public IGraphicsAPI
{
public:
    VulkanAPI(SDLManager *sdlManager = nullptr, OpenXRManager *openXRManager = nullptr);
    ~VulkanAPI() override;

    bool initAPI() override;
    Version getVersion() override;
    bool cleanup() override;
    SDL_WindowFlags getSDLWindowFlags() override;

    bool getGraphicsBinding(XrGraphicsBindingVulkanKHR &graphicsBinding);
    bool createSDLSurface();

private:
    VkInstance instance_;
    bool initialized_;

    // Manager pointers
    OpenXRManager *openXRManager_;
    SDLManager *sdlManager_;

    // Vulkan Rendering Surface
    VkSurfaceKHR SDLsurface_;

    // SDL extensions
    bool fetchSDLInstanceExtensions();
    bool printSDLInstanceExtensions();
    std::vector<const char *> sdlInstanceExtensions;

    bool fetchXRDeviceExtensions();
    bool printXRDeviceExtensions();
    std::vector<const char *> xrDeviceExtensions;
    std::vector<char> xrDeviceExtensionNames_;

    bool fetchXRInstanceExtensions();
    bool printXRInstanceExtensions();
    std::vector<const char *> xrInstanceExtensions;
    std::vector<char> xrInstanceExtensionNames_;

    bool fetchXRGraphicsRequirements();
    XrGraphicsRequirementsVulkanKHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR};

    XrInstance *xrInstanceptr_;
    XrSystemId *xrSystemIdptr_;

    // Physical device management
    std::vector<VkPhysicalDevice> physicalDevices_;
    bool fetchPhysicalDevices();
    bool printPhysicalDevices();

    VkPhysicalDevice physicalDevice_;
    VkPhysicalDevice getOpenXRRecommendedDevice();
    bool selectPhysicalDevice();

    std::vector<VkQueueFamilyProperties> queueFamiliesP_;

    uint32_t graphicsQueueFamilyIndex_;
    uint32_t computeQueueFamilyIndex_;
    uint32_t SDLPresentQueueFamilyIndex_;
    
    bool fetchPhysicalDeviceQueueProperties();
    bool printPhysicalDeviceQueueProperties();
    bool findQueue(QueueType type, uint32_t *queueFamilyIndex, bool requireSDLSupport = false);

    // Logical device
    VkDevice logicalDevice_;
    bool createLogicalDevice();
    bool printLogicalDeviceFeatures();

    bool fetchQueues();
    VkQueue graphicsQueue_;
    VkQueue computeQueue_;
    VkQueue SDLPresentQueue_;

    VkSwapchainKHR swapchain_  = VK_NULL_HANDLE;
    bool createSwapchain();

    std::vector<VkImage> swapchainImages_;
    std::vector<VkImageView> swapchainImageViews_;
    bool generateSwapchainImageViews();
};

#endif // VULKAN_LINKED
#endif // VULKANAPI_H
