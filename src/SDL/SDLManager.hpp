#ifndef SDLMANAGER_HPP
#define SDLMANAGER_HPP

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include "Utils/ApplicationInfo.h"
#include "Utils/Utils.hpp"
#include "OpenXR/XRUtils.hpp"

// handle and stores information related to SDL

class SDLManager
{
public:
    SDLManager(const ApplicationInfo &appInfo = ApplicationInfo());
    ~SDLManager();

    bool initialize();
    bool createWindow(int width, int height, Uint32 flags = SDL_WINDOW_SHOWN);
    bool destroyWindow();
    int pollEvent(SDL_Event &event);
    bool cleanup();

    SDL_Window *getWindow() { return window_; }
    SDL_Renderer *getRenderer() { return renderer_; }
    bool isRunning() const { return isRunning_; }

    bool isSDLValid();

    bool getResolution(int &width, int &height);

private:
    SDL_Window *window_;
    SDL_Renderer *renderer_;
    bool isRunning_;

    ApplicationInfo appInfo_;

    bool initSDL();
    void printSDLVersion();
    void printSDLDrivers();

    std::vector<std::string> videoDrivers_;
    bool getVideoDrivers();
    bool printVideoDrivers();
};

#endif // SDLMANAGER_HPP
