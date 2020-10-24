#ifndef FUNDAMENT_VULKAN_GRAPHICS_H
#define FUNDAMENT_VULKAN_GRAPHICS_H

#include <fundament/types.h>

#include <stdbool.h>

#include <vulkan/vulkan.h>

bool fn__init_vulkan_graphics();
void fn__deinit_vulkan_graphics();

struct fn__surface {
    VkSurfaceKHR    surface;
    VkSwapchainKHR  swapchain;
    VkImage*        images;
    uint32_t        image_count;
};

bool fn__create_vulkan_surface(
    fn_native_window_handle_t window,
    struct fn__surface* surface
);

void fn__destroy_vulkan_surface(struct fn__surface* surface);

void fn__present_vulkan_surface(struct fn__surface* surface);

#define fn__init_imp_graphics   fn__init_vulkan_graphics
#define fn__deinit_imp_graphics fn__deinit_vulkan_graphics

#endif //FUNDAMENT_VULKAN_GRAPHICS_H
