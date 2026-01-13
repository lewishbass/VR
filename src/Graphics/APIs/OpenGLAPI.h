#ifndef OPENGLAPI_H
#define OPENGLAPI_H

#include "GraphicsAPI.h"

#ifdef OPENGL_LINKED
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

class SDLManager; // Forward declaration
class OpenXRManager; // Forward declaration

class OpenGLAPI : public IGraphicsAPI {
public:
    OpenGLAPI(SDLManager* sdlManager = nullptr, OpenXRManager* openXRManager = nullptr);
    ~OpenGLAPI() override;
    
    bool initAPI() override;
    Version getVersion() override;
    bool cleanup() override;
    SDL_WindowFlags getSDLWindowFlags() override;
    bool createSDLSurface();

private:
    void* context_;
    bool initialized_;

    SDLManager* sdlManager_;
    OpenXRManager* openXRManager_;
};

#endif // OPENGL_LINKED
#endif // OPENGLAPI_H
