#ifndef FUNDAMENT_GRAPHICS_H
#define FUNDAMENT_GRAPHICS_H

//==============================================================================
//                                  GRAPHICS
//
// Provides functions and structures utilize on the underlying graphics hardware
// for rendering and computations.
//
//==============================================================================

#include <stdbool.h>

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

#endif  // FUNDAMENT_GRAPHICS_H

