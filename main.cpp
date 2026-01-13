#include <iostream>
// Define this before including SDL to prevent SDL from redefining main
#define SDL_MAIN_HANDLED
// Include SDL2 header
#include <SDL2/SDL.h>
// Define Vulkan graphics API support for OpenXR
#define XR_USE_GRAPHICS_API_VULKAN 1
// Include TinyObjLoader implementation
#define TINYOBJLOADER_IMPLEMENTATION
// Include Vulkan header
#include <vulkan/vulkan.h>
// Include OpenXR header
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>

#include "OpenXR/OpenXRManager.h"
#include "Graphics/GraphicsManager.h"
#include "OpenXR/XRUtils.hpp"
#include "Utils/Utils.hpp"

#include "Graphics/Objects/Model.hpp"
#include "Graphics/Objects/Material.hpp"

int main()
{
    // logMessage(3, "VRTestProj is starting up.");
    GraphicsManager gfxManager;

    // load sample asset
    // Model suzanne = Model("assets/suzanne.obj");

    // load sample material
    // Material simpleMaterial("testshaders", "TestMaterial");

    // return 0;

    bool running = true;
    static auto startTime = std::chrono::high_resolution_clock::now();

    while (running)
    {
        // Poll OpenXR events
        SDL_Event event;
        while (gfxManager.getSDLManager()->pollEvent(event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else
            {
                logMessage(3, "SDL Event Poll: Type " + std::to_string(event.type), {"SDL"});
            }
        }

        // Here would go rendering and other per-frame logic

        // For demonstration, we'll just run for a short time
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        if (elapsed > 5) // Run for 5 seconds
        {
            running = false;
        }
    }

    return 0;
#if FALSE

    // =============== CREATE OPENXR SESSION ===============
    XrGraphicsBindingVulkanKHR graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR};
    graphicsBinding.instance = vkInstance;
    graphicsBinding.physicalDevice = vkPhysicalDevice;
    graphicsBinding.device = vkDevice;
    graphicsBinding.queueFamilyIndex = 0;
    graphicsBinding.queueIndex = 0;

    XrSession session = XR_NULL_HANDLE;
    XrSessionCreateInfo sessionCreateInfo = {XR_TYPE_SESSION_CREATE_INFO};
    sessionCreateInfo.next = &graphicsBinding;
    sessionCreateInfo.systemId = systemId;
    result = xrCreateSession(instance, &sessionCreateInfo, &session);

    if (result != XR_SUCCESS)
    {
        dirtyXRErrorPrint(result);
        std::cout << "Failed to create OpenXR session -- Exiting" << std::endl;
        vkDestroyDevice(vkDevice, nullptr);
        vkDestroyInstance(vkInstance, nullptr);
        xrDestroyInstance(instance);
        return -1;
    }

    std::cout << "OpenXR session created successfully." << std::endl;

    // =============== CREATE REFERENCE SPACE ===============
    std::cout << "Creating reference space..." << std::endl;

    XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    referenceSpaceCreateInfo.poseInReferenceSpace = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};

    XrSpace localSpace = XR_NULL_HANDLE;
    result = xrCreateReferenceSpace(session, &referenceSpaceCreateInfo, &localSpace);
    if (result != XR_SUCCESS)
    {
        dirtyXRErrorPrint(result);
        std::cout << "Failed to create local reference space -- Exiting" << std::endl;
        xrDestroySession(session);
        vkDestroyDevice(vkDevice, nullptr);
        vkDestroyInstance(vkInstance, nullptr);
        xrDestroyInstance(instance);
        return -1;
    }
    std::cout << "Local reference space created successfully." << std::endl;

    // Create view space for tracking
    XrReferenceSpaceCreateInfo viewSpaceCreateInfo{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    viewSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
    viewSpaceCreateInfo.poseInReferenceSpace = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};

    XrSpace viewSpace = XR_NULL_HANDLE;
    result = xrCreateReferenceSpace(session, &viewSpaceCreateInfo, &viewSpace);
    if (result != XR_SUCCESS)
    {
        dirtyXRErrorPrint(result);
        std::cout << "Failed to create view reference space -- Exiting" << std::endl;
        xrDestroySpace(localSpace);
        xrDestroySession(session);
        vkDestroyDevice(vkDevice, nullptr);
        vkDestroyInstance(vkInstance, nullptr);
        xrDestroyInstance(instance);
        return -1;
    }
    std::cout << "View reference space created successfully." << std::endl;

    // =============== POLL OPENXR EVENTS ===============
    XrEventDataBuffer eventDataBuffer{XR_TYPE_EVENT_DATA_BUFFER};
    auto XrPollEvent = [&]() -> bool
    {
        eventDataBuffer = {XR_TYPE_EVENT_DATA_BUFFER};
        return xrPollEvent(instance, &eventDataBuffer) == XR_SUCCESS;
    };
    std::cout << "Polling events..." << std::endl;

    auto startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    auto lastPrintTime = startTime;
    int frameCount = 0;

    while (lastPrintTime - startTime < 30 * 1000) // Run for 30 seconds
    {
        auto now = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() - lastPrintTime >= 100)
        {
            // Get current time for space location query
            XrTime currentTime;
            XrFrameState frameState{XR_TYPE_FRAME_STATE};

            // Try to get predicted display time (may not work without frame wait)
            // For polling demo, we'll use a simple increasing time
            currentTime = frameCount * 16666667; // Approximate 60Hz frame time in nanoseconds
            frameCount++;

            // Query space location
            XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
            result = xrLocateSpace(viewSpace, localSpace, currentTime, &spaceLocation);

            std::time_t now_time = std::chrono::system_clock::to_time_t(now);
            std::tm *tm = std::localtime(&now_time);

            std::cout << "polling - "
                      << std::setfill('0') << std::setw(2) << tm->tm_hour << ":"
                      << std::setfill('0') << std::setw(2) << tm->tm_min << ":"
                      << std::setfill('0') << std::setw(2) << tm->tm_sec << "."
                      << std::setfill('0') << std::setw(3) << (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000)
                      << '\r';

            if (result == XR_SUCCESS && (spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT))
            {
                std::cout << " | Pos: ["
                          << std::fixed << std::setprecision(3)
                          << std::setw(7) << spaceLocation.pose.position.x << ", "
                          << std::setw(7) << spaceLocation.pose.position.y << ", "
                          << std::setw(7) << spaceLocation.pose.position.z << "]";

                if (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT)
                {
                    std::cout << " Ori: ["
                              << std::setw(6) << spaceLocation.pose.orientation.x << ", "
                              << std::setw(6) << spaceLocation.pose.orientation.y << ", "
                              << std::setw(6) << spaceLocation.pose.orientation.z << ", "
                              << std::setw(6) << spaceLocation.pose.orientation.w << "]";
                }
            }
            /*else
            {
                std::cout << " | Position not tracked";
            }*/

            std::cout << std::flush;
            lastPrintTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        }

        while (XrPollEvent())
        {
            std::cout << "\nPolled an OpenXR event of type: " << eventDataBuffer.type << std::endl;
        }
    }

    std::cout << "\nCleaning up resources..." << std::endl;
    xrDestroySpace(viewSpace);
    xrDestroySpace(localSpace);
    xrDestroySession(session);
    xrDestroyInstance(instance);
    vkDestroyDevice(vkDevice, nullptr);
    vkDestroyInstance(vkInstance, nullptr);
    std::cout << "Cleanup complete." << std::endl;
    return 0;
#endif
}
