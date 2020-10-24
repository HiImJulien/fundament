#ifndef FUNDAMENT_GRAPHICS_COMMON_H
#define FUNDAMENT_GRAPHICS_COMMON_H

//==============================================================================
//                              GRAPHICS COMMON
//
// Defines data structures used commonly by all graphics backends.
//
//==============================================================================

#include <fundament/handle_pool.h>

#include "vulkan/vulkan_graphics.h"

struct fn__graphics_context {
    VkInstance              instance;
    VkPhysicalDevice        adapter;
    VkDevice                device;
    uint32_t                graphics_queue_index;
    uint32_t                present_queue_index;
    VkQueue                 graphics_queue;
    VkQueue                 present_queue;

    struct fn_handle_pool   swap_chain_pool;
};

extern struct fn__graphics_context fn__g_graphics_context;

#endif  // FFUNDAMENT_GRAPHICS_COMMON_H
