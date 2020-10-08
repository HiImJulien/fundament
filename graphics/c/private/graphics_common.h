#ifndef FUNDAMENT_GRAPHICS_COMMON_H
#define FUNDAMENT_GRAPHICS_COMMON_H

//==============================================================================
// This module defines structures and functions used by both, the public and 
// private API.
//==============================================================================

#include <stddef.h>

#if defined(__APPLE__)
    #include "metal/graphics_metal.h"
#else
    #error "Unsupported platform!"
#endif

struct fn__gfx_device {
    fn__native_gfx_device_t handle;
};

struct fn__gfx_context {
    struct fn__gfx_device*  devices;
    size_t                  devices_size; 
    size_t                  devices_capacity;
};

extern struct fn__gfx_context* fn__g_gfx_context;

#endif  // FUNDAMENT_GRAPHICS_COMMON_H

