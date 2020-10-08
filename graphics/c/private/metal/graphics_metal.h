#ifndef FUNDAMENT_GRAPHICS_METAL_H
#define FUNDAMENT_GRAPHICS_METAL_H

//==============================================================================
// This module implements the public graphics API using the Metal API. 
//==============================================================================

#include <stdbool.h>

#if defined(__OBJC__)
    #define FN_FWD_PROTO_ALIAS(x, a)                                           \
        @protocol x;                                                           \
        typedef id<x> a;                                                       
#else
    #define FN_FWD_PROTO_ALIAS(x, a)                                           \
        typedef void* a;                                                       
#endif

FN_FWD_PROTO_ALIAS(MTLDevice, fn__native_gfx_device_t);

struct fn__gfx_device;

bool fn__create_gfx_device(struct fn__gfx_device* device);
void fn__destroy_gfx_device(struct fn__gfx_device* device);


#endif  // FUNDAMENT_GRAPHICS_METAL_H

