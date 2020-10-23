#include <fundament/graphics.h>

#include "graphics_common.h"

bool fn_init_graphics() {
    if(!fn__init_vulkan_graphics())
        return false;

    fn_initialize_handle_pool(
        &fn__g_graphics_context.surface_pool,
        FN_WINDOW_CAPACITY
    );

    return true;
}

void fn_deinit_graphics() {

    fn_deinitialize_handle_pool(
        &fn__g_graphics_context.surface_pool
    );

    fn__deinit_imp_graphics();
}

struct fn_surface fn_create_surface(fn_native_window_handle_t window) {
    uint32_t handle, index;
    if(!fn_alloc_handle(
        &fn__g_graphics_context.surface_pool,
        &handle,
        &index))
        return (struct fn_surface) { 0 };

    struct fn__surface* ptr = &fn__g_graphics_context.surfaces[index];
    if(!fn__create_vulkan_surface(window, ptr))
        return (struct fn_surface) { 0 };

    return (struct fn_surface) { handle };
}

void fn_destroy_surface(struct fn_surface surface) {

}