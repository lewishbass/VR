#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <functional>
#include <memory>
#include <string>

#include "APIs/GraphicsAPI.h"
#include "OpenXR/OpenXRManager.h"
#include "Utils/ApplicationInfo.h"

#include "SDL/SDLManager.hpp"

#if VULKAN_LINKED
#include "APIs/VulkanAPI.h"
#endif

#if OPENGL_LINKED
#include "APIs/OpenGLAPI.h"
#endif

#if METAL_LINKED
#include "APIs/MetalAPI.h"
#endif

// handle and stores information related to graphics API

enum class GraphicsAPI
    {
#if VULKAN_LINKED
        VULKAN,
#endif
#if OPENGL_LINKED
        OPENGL,
#endif
#if METAL_LINKED
        METAL,
#endif
        UNKNOWN
    };


inline const std::vector<std::string> GraphicsAPIToString = {
#if VULKAN_LINKED
    "Vulkan",
#endif
#if OPENGL_LINKED
    "OpenGL",
#endif
#if METAL_LINKED
    "Metal",
#endif
    "Unknown"
};

class GraphicsManager
{
public:

    GraphicsManager(GraphicsAPI api = GraphicsAPI::UNKNOWN, const ApplicationInfo& appInfo = ApplicationInfo());
    ~GraphicsManager();

    bool selectAPI(std::vector<GraphicsAPI> apiOrder);
    bool initSDLWindow();

    SDLManager* getSDLManager() { return sdlManager_.get(); }
    OpenXRManager* getOpenXRManager() { return openXRManager_.get(); }

private:
    std::vector<std::unique_ptr<IGraphicsAPI>> graphicsAPIs_;
    IGraphicsAPI* activeAPI_;
    GraphicsAPI selectedAPI_;

    ApplicationInfo appInfo_;

    bool vulkanLinked_;
    bool openGLLinked_;
    bool metalLinked_;

    bool vulkanSupported_;
    bool openGLSupported_;
    bool metalSupported_;

    bool initVulkan();
    bool initOpenGL();
    bool initMetal();

    void initializeAPIs();

    std::unique_ptr<OpenXRManager> openXRManager_;
    std::unique_ptr<SDLManager> sdlManager_;


};

#endif // GRAPHICSMANAGER_H

// OpenXR instance
// Graphics APIinstance
// SDL window
// Graphics API init
// OpenXR session