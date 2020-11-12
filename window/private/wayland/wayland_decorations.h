#ifndef FUNDAMENT_WAYLAND_DECORATIONS_H
#define FUNDAMENT_WAYLAND_DECORATIONS_H

//==============================================================================
//                            WAYLAND DECORATIONS
//
// Provides utilities to render custom decorations for wayland windows.
//
// The wayland protocol does not acknowledge window decorations directly.
// Decorations are instead implemented as an extension, namely 
// 'zxdg_decoration_manager_v1', which is sadly not implemented by some window
// managers/compositors including GNOME's "Mutter".
//
//==============================================================================

#include <stdint.h>

void fn__init_wayland_decorations();
void fn__deinit_wayland_decorations();

typedef struct fn__window fn__window;
void fn__decorate_wayland_window(fn__window* window, uint32_t* image_data);

#endif  // FUNDAMENT_WAYLAND_DECORATIONS_H

