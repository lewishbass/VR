#include "SDLManager.hpp"

SDLManager::SDLManager(const ApplicationInfo& appInfo)
    : window_(nullptr), renderer_(nullptr), 
      isRunning_(false), appInfo_(appInfo)
{
    if (!this->initialize())
        logMessage(1, "Failed to initialize SDLManager.", {"SDL", "Error"});
    else
        logMessage(3, "SDLManager initialized.", {"SDL"});
}

SDLManager::~SDLManager()
{
    this->cleanup();
    logMessage(3, "SDLManager destroyed.", {"SDL"});
}

bool SDLManager::initialize()
{
    if (!initSDL())
        return false;
    printSDLVersion();
    if (!getVideoDrivers())
        return false;
    printVideoDrivers();
    return true;
}

bool SDLManager::initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        std::stringstream ss;
        ss << "Failed to initialize SDL: " << SDL_GetError();
        logMessage(2, ss.str(), {"Graphics", "SDL"});
        return false;
    }

    logMessage(3, "SDL initialized successfully.", {"Graphics", "SDL"});
    isRunning_ = true;
    return true;
}

bool SDLManager::createWindow(int width, int height, Uint32 flags)
{
    if (window_ != nullptr)
    {
        logMessage(2, "Window already exists.", {"Graphics", "SDL"});
        return false;
    }

    window_ = SDL_CreateWindow(
        appInfo_.appName.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        flags
    );

    if (window_ == nullptr)
    {
        std::stringstream ss;
        ss << "Failed to create window: " << SDL_GetError();
        logMessage(2, ss.str(), {"Graphics", "SDL"});
        return false;
    }

    logMessage(3, "SDL window created successfully.", {"Graphics", "SDL"});
    return true;
}

bool SDLManager::destroyWindow()
{
    if (window_ != nullptr)
    {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        logMessage(3, "SDL window destroyed.", {"SDL"});
        return true;
    }
    return false;
}

void SDLManager::printSDLVersion()
{
    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    std::stringstream ss;
    ss << "SDL Version Information:\n";
    ss << "  Compiled against SDL version: " 
       << (int)compiled.major << "." 
       << (int)compiled.minor << "." 
       << (int)compiled.patch << "\n";
    ss << "  Linked SDL version: " 
       << (int)linked.major << "." 
       << (int)linked.minor << "." 
       << (int)linked.patch << "\n";

    logMessage(4, ss.str(), {"Graphics", "SDL"});
}

bool SDLManager::getVideoDrivers()
{
    int driverCount = SDL_GetNumVideoDrivers();
    
    if (driverCount < 0)
    {
        std::stringstream ss;
        ss << "Failed to get video driver count: " << SDL_GetError();
        logMessage(2, ss.str(), {"Graphics", "SDL"});
        return false;
    }

    videoDrivers_.clear();
    for (int i = 0; i < driverCount; ++i)
    {
        const char* driverName = SDL_GetVideoDriver(i);
        if (driverName != nullptr)
        {
            videoDrivers_.push_back(driverName);
        }
    }

    logMessage(3, "SDL video drivers enumerated successfully.", {"Graphics", "SDL"});
    return true;
}

bool SDLManager::printVideoDrivers()
{
    std::stringstream ss;
    ss << "\n=== Available SDL Video Drivers ===\n";
    ss << "Total video drivers available: " << videoDrivers_.size() << "\n";

    if (videoDrivers_.empty())
    {
        ss << "(No video drivers found)\n";
        logMessage(4, ss.str(), {"Graphics", "SDL"});
        return true;
    }

    const char* currentDriver = SDL_GetCurrentVideoDriver();
    ss << "Current video driver: " << (currentDriver ? currentDriver : "None") << "\n\n";

    ss << "Available video drivers:\n";
    for (const auto& driver : videoDrivers_)
    {
        ss << "  - " << driver;
        if (currentDriver && driver == currentDriver)
        {
            ss << " (ACTIVE)";
        }
        ss << "\n";
    }

    logMessage(4, ss.str(), {"Graphics", "SDL"});
    return true;
}

int SDLManager::pollEvent(SDL_Event &event)
{
    return SDL_PollEvent(&event);
}

bool SDLManager::cleanup()
{
    // dispose of all SDL resources
    if (renderer_ != nullptr)
    {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_ != nullptr)
    {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
    isRunning_ = false;
    return true;
}

bool SDLManager::isSDLValid()
{
    if (window_ == nullptr)
    {
        logMessage(4, "SDL validation failed: Window is null", {"SDL", "Validation"});
        return false;
    }
    
    return true;
}

bool SDLManager::getResolution(int &width, int &height)
{
    if (window_ == nullptr)
    {
        logMessage(2, "Cannot get resolution: SDL window is null.", {"Graphics", "SDL"});
        return false;
    }

    SDL_GetWindowSize(window_, &width, &height);
    //logMessage(3, "Fetched SDL window resolution: " + std::to_string(width) + "x" + std::to_string(height), {"Graphics", "SDL"});
    return true;
}