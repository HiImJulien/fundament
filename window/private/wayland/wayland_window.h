#ifndef FUNDAMENT_WAYLAND_WINDOW_H
#define FUNDAMENT_WAYLAND_WINDOW_H

//==============================================================================
//                               WAYLAND WINDOW
//
// Implements windows using the wayland API. Renders its own window decorations
// unless the "zxdg_decoration_manager_v1" interface is present.
//
//==============================================================================

#include <stdbool.h>
#include <stdint.h>

typedef struct fn__window fn__window;

bool fn__init_wayland_window();
void fn__deinit_wayland_window();

bool fn__create_wayland_window(fn__window* window);
void fn__destroy_wayland_window(fn__window* window);

void fn__wayland_window_set_size(
    fn__window* window, 
    uint32_t width, 
    uint32_t height
);

void fn__wayland_window_set_title(fn__window* window, const char* title);
void fn__wayland_window_set_visible(fn__window* window, bool visible);

void fn__wayland_pump_events();

#endif  // FUNDAMENT_WAYLAND_WINDOW_H

