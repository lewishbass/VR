#ifndef GRAPHICSAPI_H
#define GRAPHICSAPI_H

#include <cstdint>

#if VULKAN_LINKED
#define XR_USE_GRAPHICS_API_VULKAN 1
#include <vulkan/vulkan.h>
#endif

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <SDL2/SDL.h>

struct Version {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

class IGraphicsAPI {
public:
    virtual ~IGraphicsAPI() = default;
    
    virtual bool initAPI() = 0;
    virtual Version getVersion() = 0;

    virtual bool cleanup() = 0;
    virtual SDL_WindowFlags getSDLWindowFlags() = 0;
};

#endif // GRAPHICSAPI_H
