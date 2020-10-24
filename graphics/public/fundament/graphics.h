#ifndef FUNDAMENT_GRAPHICS_H
#define FUNDAMENT_GRAPHICS_H

//==============================================================================
//                                  GRAPHICS
//
// Provides functions and structures utilize on the underlying graphics hardware
// for rendering and computations.
//
//==============================================================================

#include <fundament/types.h>

#include <stdbool.h>
#include <stdint.h>

//
// Initializes the graphics module,
// allocating internal structures etc.
//
bool fn_init_graphics();

//
// Deinitializes the graphics module,
// releasing resources allocated through fn_init_graphics.
//
void fn_deinit_graphics();

struct fn_swap_chain{ uint32_t id; };
struct fn_swap_chain_desc {
    fn_native_window_handle_t   window;
    uint32_t                    buffer_count;
};

//
//
//
struct fn_swap_chain fn_create_swap_chain();

//
//
//
void fn_destroy_swap_chain(struct fn_swap_chain swap_chain);

#endif  // FUNDAMENT_GRAPHICS_H

