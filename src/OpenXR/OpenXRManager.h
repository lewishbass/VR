#ifndef OPENXRMANAGER_H
#define OPENXRMANAGER_H

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <iostream>
#include <string>
#include <vector>
#include "Utils/ApplicationInfo.h"
#include "Utils/Utils.hpp"
#include "OpenXR/XRUtils.hpp"

// handle and stores information related to OpenXR


class OpenXRManager
{
public:
    OpenXRManager(const ApplicationInfo &appInfo = ApplicationInfo());
    ~OpenXRManager();

    bool initialize();
    bool createSession();
    bool pollEvents();
    bool cleanup();

    XrInstance *getInstance() { return &instance_; }
    XrSystemId *getSystemId() { return &systemId_; }
    bool isXRValid();

private:
    XrInstance instance_;
    XrSpace localSpace_;
    XrSpace viewSpace_;
    XrSystemId systemId_;

    ApplicationInfo appInfo_;

    bool initOpenXR();
    bool initOpenXrSystem();
    void printXrSystemInfo();

    std::vector<XrExtensionProperties> xrExtensions_;
    bool getXrExtensions();
    bool printXrExtensions();

    std::vector<XrApiLayerProperties> xrApiLayers_;
    bool getXrApiLayers();
    bool printXrApiLayers();

    
    XrSession session_;
    bool createXrSession();
    bool printXrSessionInfo();
};


#endif // OPENXRMANAGER_H
