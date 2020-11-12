#include "window_backend.h"
#include "../wayland/wayland_window.h"
#include "../xcb/xcb_window.h"

#include <stdlib.h>

static const fn__window_backend fn__g_wayland_backend = {
    .init_backend   = fn__init_wayland_window,
    .deinit_backend = fn__deinit_wayland_window,
    .create_window  = fn__create_wayland_window,
    .destroy_window = fn__destroy_wayland_window,
    .set_size       = fn__wayland_window_set_size,
    .set_title      = fn__wayland_window_set_title,
    .set_visible    = fn__wayland_window_set_visible,
    .pump_events    = fn__wayland_pump_events
};

static const fn__window_backend fn__g_xcb_backend = {
    .init_backend   = fn__init_xcb_window,
    .deinit_backend = fn__deinit_xcb_window,
    .create_window  = fn__create_xcb_window,
    .destroy_window = fn__destroy_xcb_window,
    .set_size       = fn__xcb_window_set_size,
    .set_title      = fn__xcb_window_set_title,
    .set_visible    = fn__xcb_window_set_visible,
    .pump_events    = fn__xcb_pump_events
}; 

const fn__window_backend* fn__g_window_backend = &fn__g_xcb_backend;

bool fn__init_imp_window() {
    const char* wayland_display = getenv("WAYLAND_DISPLAY");
    if(wayland_display)
        fn__g_window_backend = &fn__g_wayland_backend;

    return fn__g_window_backend->init_backend();
}
