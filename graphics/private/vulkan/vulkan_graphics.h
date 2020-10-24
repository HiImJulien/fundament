#ifndef FUNDAMENT_VULKAN_GRAPHICS_H
#define FUNDAMENT_VULKAN_GRAPHICS_H

#include <stdbool.h>

#include <vulkan/vulkan.h>

bool fn__init_vulkan_graphics();
void fn__deinit_vulkan_graphics();

#define fn__init_imp_graphics   fn__init_vulkan_graphics
#define fn__deinit_imp_graphics fn__deinit_vulkan_graphics

#endif  // FFUNDAMENT_VULKAN_GRAPHICS_H
