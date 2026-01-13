#include "MetalAPI.h"
#include "../../Utils/Utils.hpp"

#ifdef METAL_LINKED
    MetalAPI::MetalAPI() : initialized_(false) {
        device_ = nil;
        commandQueue_ = nil;
    }

    MetalAPI::~MetalAPI() {
        cleanup();
    }

    bool MetalAPI::initAPI() {
        // logMessage(3, "Initializing Metal API...", {"Graphics", "Metal"});
        
        device_ = MTLCreateSystemDefaultDevice();
        if (!device_) {
            logMessage(1, "Failed to create Metal device", {"Graphics", "Metal", "Error"});
            return false;
        }
        
        commandQueue_ = [device_ newCommandQueue];
        if (!commandQueue_) {
            logMessage(1, "Failed to create Metal command queue", {"Graphics", "Metal", "Error"});
            return false;
        }
        
        initialized_ = true;
        return true;
    }

    Version MetalAPI::getVersion() {
        // Metal versioning is tied to OS version
        // This is a simplified example
        return Version{3, 0, 0};
    }

    bool MetalAPI::cleanup() {
        if (!initialized_) {
            return true;
        }
        
        logMessage(3, "Cleaning up Metal API...", {"Graphics", "Metal"});
        
        if (commandQueue_) {
            [commandQueue_ release];
            commandQueue_ = nil;
        }
        
        if (device_) {
            [device_ release];
            device_ = nil;
        }
        
        initialized_ = false;
        return true;
    }
#endif // METAL_LINKED
