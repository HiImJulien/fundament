#ifndef FUNDAMENT_GRAPHICS_COMMON_H
#define FUNDAMENT_GRAPHICS_COMMON_H

//==============================================================================
//                              GRAPHICS COMMON
//
// Defines data structures used commonly by all graphics backends.
//
//==============================================================================

#include <fundament/types.h>
#include <fundament/handle_pool.h>

#include "vulkan/vulkan_graphics.h"

struct fn__graphics_context {
    VkInstance              instance;
    VkPhysicalDevice        adapter;
    VkDevice                device;
    uint32_t                graphics_queue_index;
    uint32_t                present_queue_index;
    VkSemaphore             image_sem;
    VkSemaphore             render_sem;
    VkQueue                 graphics_queue;
    VkQueue                 present_queue;

    struct fn_handle_pool   swap_chain_pool;
    fn__swap_chain_t        swap_chains[FN_WINDOW_CAPACITY];
    struct fn_handle_pool   texture_pool;
    fn__texture_t           textures[FN_TEXTURE_CAPACITY];
    struct fn_handle_pool   command_list_pool;
    fn__command_list_t      command_lists[FN_COMMAND_LIST_CAPACITY];
    struct fn_handle_pool   shader_pool;
    fn__shader_t            shaders[FN_SHADER_CAPACITY];
};

extern struct fn__graphics_context fn__g_graphics_context;

#endif  // FFUNDAMENT_GRAPHICS_COMMON_H
