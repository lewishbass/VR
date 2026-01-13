#include "Graphics/GraphicsManager.h"
#include "Utils/Utils.hpp"
#include <sstream>

GraphicsManager::GraphicsManager(GraphicsAPI api, const ApplicationInfo &appInfo) : appInfo_(appInfo)
{

    this->vulkanLinked_ = VULKAN_LINKED;
    this->openGLLinked_ = OPENGL_LINKED;
    this->metalLinked_ = METAL_LINKED;

    openXRManager_ = std::make_unique<OpenXRManager>(appInfo);
    sdlManager_ = std::make_unique<SDLManager>(appInfo);

    if (!appInfo_.desktopWindow && !openXRManager_->isXRValid())
    {
        appInfo_.desktopWindow = true;
        logMessage(1, "OpenXR not valid falling back to SDL window", {"Graphics", "Error"});
    }

    std::stringstream ss;
    ss << "Graphics APIs Linked\n    - Vulkan: " << (vulkanLinked_ ? "Yes" : "No")
       << "\n    - OpenGL: " << (openGLLinked_ ? "Yes" : "No")
       << "\n    - Metal: " << (metalLinked_ ? "Yes" : "No");
    logMessage(3, ss.str(), {"Graphics", "Initialization"});

    if (!vulkanLinked_ && !openGLLinked_ && !metalLinked_)
        logMessage(1, "No supported graphics APIs detected!", {"Graphics", "Error"});

    initializeAPIs();

    // initialized APIs print versions
    ss.str("");
    ss << "Initialized Graphics APIs Versions:\n";
    for (const auto &gfxAPI : graphicsAPIs_)
    {
        Version ver = gfxAPI->getVersion();
        int apiIndex = &gfxAPI - &graphicsAPIs_[0];
        ss << "    - " << GraphicsAPIToString[apiIndex] << ": "
           << ver.major << "." << ver.minor << "." << ver.patch << "\n";
    }
    logMessage(4, ss.str(), {"Graphics", "Version"});

    std::vector<GraphicsAPI> apiOrder;

    if (api != GraphicsAPI::UNKNOWN)
        apiOrder.push_back(api);

    for (const auto &availableAPI : GraphicsAPIToString)
    {
        GraphicsAPI apiEnum = static_cast<GraphicsAPI>(&availableAPI - &GraphicsAPIToString[0]);
        if (apiEnum != api)
        {
            apiOrder.push_back(apiEnum);
        }
    }
    ss.str("");
    ss << "Graphics API selection order: ";
    for (const auto &apiEnum : apiOrder)
    {
        int apiIndex = static_cast<int>(apiEnum);
        ss << GraphicsAPIToString[apiIndex] << " ";
    }
    logMessage(4, ss.str(), {"Graphics", "Initialization"});
    this->selectAPI(apiOrder);
    //openXRManager_->createSession();
}

GraphicsManager::~GraphicsManager()
{
    graphicsAPIs_.clear();
    activeAPI_ = nullptr;
    openXRManager_.reset();
    logMessage(3, "GraphicsManager destroyed.", {"Graphics"});
}

void GraphicsManager::initializeAPIs()
{
#if VULKAN_LINKED
    graphicsAPIs_.push_back(std::make_unique<VulkanAPI>(
        sdlManager_.get(),
        openXRManager_.get()));
    logMessage(4, "VulkanAPI instance created", {"Graphics", "Vulkan"});
#endif

#if OPENGL_LINKED
    graphicsAPIs_.push_back(std::make_unique<OpenGLAPI>(
        sdlManager_.get(),
        openXRManager_.get()));
    logMessage(4, "OpenGLAPI instance created", {"Graphics", "OpenGL"});
#endif

#if METAL_LINKED
    graphicsAPIs_.push_back(std::make_unique<MetalAPI>());
    logMessage(4, "MetalAPI instance created", {"Graphics", "Metal"});
#endif
}

bool GraphicsManager::selectAPI(std::vector<GraphicsAPI> apiOrder)
{
    for (const auto &api : apiOrder)
    {
        int apiIndex = static_cast<int>(api);
        logMessage(3, "Attempting to select graphics API: " + GraphicsAPIToString[apiIndex] + " (" + std::to_string(apiIndex) + ")", {"Graphics", "Selection"});
        if (apiIndex > graphicsAPIs_.size())
        {
            logMessage(2, "Requested graphics API index out of range: " + std::to_string(apiIndex), {"Graphics", "Selection"});
            continue;
        }
        IGraphicsAPI *candidateAPI = graphicsAPIs_[apiIndex].get();
        if (appInfo_.desktopWindow)
        {
            SDL_WindowFlags flags = static_cast<SDL_WindowFlags>(candidateAPI->getSDLWindowFlags() | appInfo_.sdlWindowFlags);
            if (!sdlManager_->createWindow(appInfo_.windowWidth, appInfo_.windowHeight, flags))
            {
                logMessage(2, "Failed to create window for graphics API: " + GraphicsAPIToString[apiIndex], {"Graphics", "Selection"});
                continue;
            }
        }
        // TODO: add openxr session creation here if needed
        if (candidateAPI->initAPI())
        {
            activeAPI_ = candidateAPI;
            selectedAPI_ = api;
            logMessage(4, "Selected graphics API: " + GraphicsAPIToString[apiIndex], {"Graphics", "Selection"});
            return true;
        }
        else
        {
            if (appInfo_.desktopWindow || !openXRManager_->isXRValid())
            {
                sdlManager_->destroyWindow();
            }
            logMessage(2, "Failed to initialize graphics API: " + GraphicsAPIToString[apiIndex], {"Graphics", "Selection"});
        }
    }
    logMessage(2, "Failed to select any graphics API", {"Graphics", "Warning"});
    return false;
}
