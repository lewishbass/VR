#ifndef APPLICATIONINFO_H
#define APPLICATIONINFO_H

#include <string>
#include <SDL2/SDL.h>

struct ApplicationInfo {
    std::string appName;
    int appVersion;
    std::string engineName;
    int engineVersion;
    bool desktopWindow;
    int windowWidth = 1280;
    int windowHeight = 720;
    Uint32 sdlWindowFlags = SDL_WINDOW_SHOWN;

    ApplicationInfo(const std::string& appName = "VRTestProj",
                    int appVersion = 0,
                    const std::string& engineName = "OfCourseIStillDrawYou",
                    int engineVersion = 0,
                    bool desktopWindow = false)
        : appName(appName)
        , appVersion(appVersion)
        , engineName(engineName)
        , engineVersion(engineVersion)
        , desktopWindow(desktopWindow)
    {}
};

#endif // APPLICATIONINFO_H
