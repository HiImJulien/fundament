#ifndef FUNDAMENT_GRAPHICS_H
#define FUNDAMENT_GRAPHICS_H

//==============================================================================
//                                  GRAPHICS
//
// Provides functionality to interact with GPU.
//
// TODO:
// - Vulkan backend
// - Metal backend
// - Direct3D12 backend
// - Allow multiple devices; currently one is choosen and used implicitly.
//
//==============================================================================

#include <fundament/types.h>

#include <stdbool.h>
#include <stdint.h>

bool fn_init_graphics();

void fn_deinit_graphics();

struct fn_surface{ uint32_t id; };

//
// Creates a surface for given window.
//
struct fn_surface fn_create_surface(fn_native_window_handle_t handle);

void fn_destroy_surface(struct fn_surface surface);

#endif //FUNDAMENT_GRAPHICS_H
