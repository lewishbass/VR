#include "OpenGLAPI.h"

#ifdef OPENGL_LINKED
#include "../../Utils/Utils.hpp"

OpenGLAPI::OpenGLAPI(SDLManager* sdlManager, OpenXRManager* openXRManager) 
    : initialized_(false), sdlManager_(sdlManager), openXRManager_(openXRManager)
{
    context_ = nullptr;
}

OpenGLAPI::~OpenGLAPI() {
    cleanup();
}

bool OpenGLAPI::initAPI() {
    // logMessage(3, "Initializing OpenGL API...", {"Graphics", "OpenGL"});
    
    // TODO: Initialize OpenGL context
    
    initialized_ = true;
    return true;
}

Version OpenGLAPI::getVersion() {
    const GLubyte* versionString = glGetString(GL_VERSION);
    if(versionString) {
        std::string version(reinterpret_cast<const char*>(versionString));
        int major = 0, minor = 0, patch = 0;
        sscanf(version.c_str(), "%d.%d.%d", &major, &minor, &patch);
        return Version{static_cast<uint32_t>(major), static_cast<uint32_t>(minor), static_cast<uint32_t>(patch)};
    }
    logMessage(2, "Failed to retrieve OpenGL version string.", {"Graphics", "OpenGL"});
    
    return Version{0, 0, 0};
}

SDL_WindowFlags OpenGLAPI::getSDLWindowFlags() {
    return SDL_WINDOW_OPENGL;
}

bool OpenGLAPI::cleanup() {
    if (!initialized_) {
        return true;
    }
    
    logMessage(3, "Cleaning up OpenGL API...", {"Graphics", "OpenGL"});
    
    // TODO: Cleanup OpenGL resources and context
    
    initialized_ = false;
    return true;
}

bool OpenGLAPI::createSDLSurface() {
    // OpenGL doesn't use surfaces like Vulkan
    return false;
}

#endif // OPENGL_LINKED
