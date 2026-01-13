#ifndef METALAPI_H
#define METALAPI_H

#include "GraphicsAPI.h"

#if METAL_LINKED
#include <Metal/Metal.h>

class MetalAPI : public IGraphicsAPI {
public:
    MetalAPI();
    ~MetalAPI() override;
    
    bool initAPI() override;
    Version getVersion() override;
    bool cleanup() override;

private:
    id<MTLDevice> device_;
    id<MTLCommandQueue> commandQueue_;
    bool initialized_;
};

#endif // METAL_LINKED
#endif // METALAPI_H
