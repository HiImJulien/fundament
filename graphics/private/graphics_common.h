#ifndef FUNDAMENT_GRAPHICS_COMMON_H
#define FUNDAMENT_GRAPHICS_COMMON_H

#include <fundament/handle_pool.h>

#if defined(FN_USE_VULKAN)
    #include "vulkan/vulkan_graphics.h"
#endif

struct fn__graphics_context {
    #if defined(FN_USE_VULKAN)
        VkInstance          instance;
        VkPhysicalDevice    physical_device;
        uint32_t            queue_family_index;
        VkDevice            device;
        VkQueue             queue;
    #endif

    struct fn__surface      surfaces[FN_WINDOW_CAPACITY];
    struct fn_handle_pool   surface_pool;
};

extern struct fn__graphics_context fn__g_graphics_context;

#endif //FUNDAMENT_GRAPHICS_COMMON_H
