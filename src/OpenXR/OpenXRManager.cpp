#include "OpenXRManager.h"
#include <sstream>
#include <cstring>

OpenXRManager::OpenXRManager(const ApplicationInfo& appInfo)
    : instance_(XR_NULL_HANDLE), session_(XR_NULL_HANDLE), 
      localSpace_(XR_NULL_HANDLE), viewSpace_(XR_NULL_HANDLE),
      systemId_(XR_NULL_SYSTEM_ID), appInfo_(appInfo)
{
    bool success = this->initialize();
    if (!success)
        logMessage(1, "Failed to initialize OpenXRManager.", {"OpenXR", "Error"});
    else
        logMessage(3, "OpenXRManager initialized.", {"OpenXR"});
}

OpenXRManager::~OpenXRManager()
{
    this->cleanup();
    logMessage(3, "OpenXRManager destroyed.", {"OpenXR"});
}

bool OpenXRManager::initialize()
{
    if (!initOpenXR())
        return false;
    if (!initOpenXrSystem())
        return false;
    printXrSystemInfo();
    if (!getXrExtensions())
        return false;
    printXrExtensions();
    if (!getXrApiLayers())
        return false;
    printXrApiLayers();
    return true;
}

bool OpenXRManager::initOpenXR(){
    const char *XRextensions[] = {
#if VULKAN_LINKED
        "XR_KHR_vulkan_enable"
#endif
    };
    uint32_t XRextensionCount = 0;
#if VULKAN_LINKED
    XRextensionCount++;
#endif

    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    createInfo.applicationInfo.apiVersion = XR_MAKE_VERSION(1, 0, 0);
    strcpy(createInfo.applicationInfo.applicationName, appInfo_.appName.c_str());
    createInfo.applicationInfo.applicationVersion = appInfo_.appVersion;
    strcpy(createInfo.applicationInfo.engineName, appInfo_.engineName.c_str());
    createInfo.applicationInfo.engineVersion = appInfo_.engineVersion;

    createInfo.enabledExtensionCount = XRextensionCount;
    createInfo.enabledExtensionNames = XRextensions;

    XrResult initResult = xrCreateInstance(&createInfo, &instance_);

    if (initResult == XR_SUCCESS){
        logMessage(3, "OpenXR instance created successfully.", {"Graphics", "OpenXR"});
        return true;
    }else{
        logMessage(2, "Failed to create OpenXR instance.", {"Graphics", "OpenXR"});
        dirtyXRErrorPrint(initResult, instance_);
    }

    return false;
}

bool OpenXRManager::initOpenXrSystem(){
    if (instance_ == XR_NULL_HANDLE){
        logMessage(2, "Cannot initialize OpenXR system: XR instance is NULL_HANDLE.", {"Graphics", "OpenXR"});
        return false;
    }
    XrSystemGetInfo systemGetInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    XrResult sysResult = xrGetSystem(instance_, &systemGetInfo, &systemId_);
    if (sysResult == XR_SUCCESS){
        logMessage(3, "OpenXR system obtained successfully.", {"Graphics", "OpenXR"});
        return true;
    }else{
        logMessage(2, "Failed to get OpenXR system.", {"Graphics", "OpenXR"});
        dirtyXRErrorPrint(sysResult, instance_);
    }

    return false;
}

void OpenXRManager::printXrSystemInfo(){
    XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
    XrResult propResult = xrGetSystemProperties(instance_, systemId_, &systemProperties);
    if (propResult == XR_SUCCESS){
        std::stringstream ss;
        ss << "System ID: " << systemId_ << "\n";
        ss << "System Name: " << systemProperties.systemName << "\n";
        ss << "Vendor ID: " << systemProperties.vendorId << "\n";
        ss << "System Tracking Properties: " << "\n";
        ss << "  Orientation Tracking: " << (systemProperties.trackingProperties.orientationTracking ? "Yes" : "No") << "\n";
        ss << "  Position Tracking: " << (systemProperties.trackingProperties.positionTracking ? "Yes" : "No") << "\n";
        ss << "Graphics Properties: " << "\n";
        ss << "  Max Swapchain Image Width: " << systemProperties.graphicsProperties.maxSwapchainImageWidth << "\n";
        ss << "  Max Swapchain Image Height: " << systemProperties.graphicsProperties.maxSwapchainImageHeight << "\n";
        ss << "  Max Layer Count: " << systemProperties.graphicsProperties.maxLayerCount << "\n";
        logMessage(4, ss.str(), {"Graphics", "OpenXR"});
    }else{
        logMessage(2, "Failed to get OpenXR system properties.", {"Graphics", "OpenXR"});
        dirtyXRErrorPrint(propResult, instance_);
    }
}

bool OpenXRManager::getXrExtensions(){
    uint32_t extensionCount = 0;
    XrResult enumResult = xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr);
    
    if (enumResult != XR_SUCCESS){
        logMessage(2, "Failed to enumerate extension count.", {"Graphics", "OpenXR"});
        dirtyXRErrorPrint(enumResult, instance_);
        return false;
    }

    xrExtensions_.resize(extensionCount, {XR_TYPE_EXTENSION_PROPERTIES});
    enumResult = xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, xrExtensions_.data());

    if (enumResult == XR_SUCCESS){
        logMessage(3, "OpenXR extensions enumerated successfully.", {"Graphics", "OpenXR"});
        return true;
    }else{
        logMessage(2, "Failed to enumerate OpenXR extensions.", {"Graphics", "OpenXR"});
        dirtyXRErrorPrint(enumResult, instance_);
        return false;
    }
}

bool OpenXRManager::printXrExtensions(){
    if (xrExtensions_.empty()){
        logMessage(2, "No extensions to print.", {"Graphics", "OpenXR"});
        return false;
    }

    std::stringstream ss;
    ss << "\n=== Available OpenXR Extensions ===\n";
    ss << "Total extensions available: " << xrExtensions_.size() << "\n\n";
    ss << "Graphics API Extensions:\n";

    bool hasVulkan = false;
    bool hasOpenGL = false;
    bool hasD3D11 = false;
    bool hasD3D12 = false;

    for (const auto &ext : xrExtensions_){
        std::string extName = ext.extensionName;

        if (extName.find("vulkan") != std::string::npos){
            ss << "  [VULKAN] " << ext.extensionName << " (v" << ext.extensionVersion << ")\n";
            hasVulkan = true;
        }
        else if (extName.find("opengl") != std::string::npos){
            ss << "  [OPENGL] " << ext.extensionName << " (v" << ext.extensionVersion << ")\n";
            hasOpenGL = true;
        }
        else if (extName.find("d3d11") != std::string::npos){
            ss << "  [D3D11]  " << ext.extensionName << " (v" << ext.extensionVersion << ")\n";
            hasD3D11 = true;
        }
        else if (extName.find("d3d12") != std::string::npos){
            ss << "  [D3D12]  " << ext.extensionName << " (v" << ext.extensionVersion << ")\n";
            hasD3D12 = true;
        }
    }

    ss << "\nSupported Graphics Backends:\n";
    if (hasVulkan)
        ss << "  - Vulkan\n";
    if (hasOpenGL)
        ss << "  - OpenGL\n";
    if (hasD3D11)
        ss << "  - Direct3D 11\n";
    if (hasD3D12)
        ss << "  - Direct3D 12\n";
    
    if (!hasVulkan && !hasOpenGL && !hasD3D11 && !hasD3D12){
        ss << "  (No standard graphics backends found)\n";
    }

    ss << "\nAll available extensions:\n";
    for (const auto &ext : xrExtensions_){
        ss << "  " << ext.extensionName << " (v" << ext.extensionVersion << ")\n";
    }

    logMessage(4, ss.str(), {"Graphics", "OpenXR"});
    return true;
}

bool OpenXRManager::getXrApiLayers(){
    uint32_t layerCount = 0;
    XrResult layerResult = xrEnumerateApiLayerProperties(0, &layerCount, nullptr);
    
    if (layerResult != XR_SUCCESS){
        logMessage(2, "Failed to enumerate API layer count.", {"Graphics", "OpenXR"});
        dirtyXRErrorPrint(layerResult, instance_);
        return false;
    }

    if (layerCount > 0){
        xrApiLayers_.resize(layerCount, {XR_TYPE_API_LAYER_PROPERTIES});
        layerResult = xrEnumerateApiLayerProperties(layerCount, &layerCount, xrApiLayers_.data());

        if (layerResult == XR_SUCCESS){
            logMessage(3, "OpenXR API layers enumerated successfully.", {"Graphics", "OpenXR"});
            return true;
        }else{
            logMessage(2, "Failed to enumerate OpenXR API layers.", {"Graphics", "OpenXR"});
            dirtyXRErrorPrint(layerResult, instance_);
            return false;
        }
    }else{
        logMessage(3, "No OpenXR API layers available.", {"Graphics", "OpenXR"});
        return true;
    }
}

bool OpenXRManager::printXrApiLayers(){
    std::stringstream ss;
    ss << "\n=== Available OpenXR API Layers/Runtimes ===\n";
    ss << "Total API layers available: " << xrApiLayers_.size() << "\n";

    if (xrApiLayers_.empty()){
        ss << "(No API layers found)\n";
        logMessage(4, ss.str(), {"Graphics", "OpenXR"});
        return true;
    }

    ss << "\nAvailable OpenXR API Layers:\n";
    for (const auto &layer : xrApiLayers_){
        ss << "  Layer: " << layer.layerName << "\n";
        ss << "    Description: " << layer.description << "\n";
        ss << "    Layer Version: " << layer.layerVersion << "\n";
        ss << "    Spec Version: "
           << XR_VERSION_MAJOR(layer.specVersion) << "."
           << XR_VERSION_MINOR(layer.specVersion) << "."
           << XR_VERSION_PATCH(layer.specVersion) << "\n\n";
    }

    logMessage(4, ss.str(), {"Graphics", "OpenXR"});
    return true;
}


bool OpenXRManager::createXrSession(){
    // Needs:
    // - vk instance
    // - vk physical device
    // - vk device
    // - xr systemID
    // - xr instance
    // TODO : Implement session creation
    return false;
}

bool OpenXRManager::printXrSessionInfo(){
    // TODO : Implement session info printing
    return false;
}


bool OpenXRManager::cleanup()
{
    // dispose of all OpenXR resources
    if (viewSpace_ != XR_NULL_HANDLE)
        xrDestroySpace(viewSpace_);
    if (localSpace_ != XR_NULL_HANDLE)
        xrDestroySpace(localSpace_);
    if (session_ != XR_NULL_HANDLE)
        xrDestroySession(session_);
    if (instance_ != XR_NULL_HANDLE)
        xrDestroyInstance(instance_);
    return true;
}

bool OpenXRManager::isXRValid()
{
    if (instance_ == XR_NULL_HANDLE) {
        logMessage(4, "XR validation failed: Instance is invalid", {"OpenXR", "Validation"});
        return false;
    }
    
    if (systemId_ == XR_NULL_SYSTEM_ID) {
        logMessage(4, "XR validation failed: System ID is invalid", {"OpenXR", "Validation"});
        return false;
    }
    
    return true;
}