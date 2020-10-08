#include "../graphics_common.h"
#include "graphics_metal.h"

#include <Metal/Metal.h>

bool fn__create_gfx_device(struct fn__gfx_device* device) {
    device->handle = MTLCreateSystemDefaultDevice();
    return device->handle != nil;
}

void fn__destroy_gfx_device(struct fn__gfx_device* device) {
    [device->handle release];
}
