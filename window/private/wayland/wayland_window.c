#include "wayland_window.h"
#include "../window_common.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <wayland-client.h>
#include "xdg_shell.h"

//==============================================================================
//                             INTERNAL FUNCTIONS
//==============================================================================

static struct wl_display*       fn__g_display       = NULL;
static struct wl_compositor*    fn__g_compositor    = NULL;
static struct wl_shm*           fn__g_shm           = NULL;
static struct wl_seat*          fn__g_seat          = NULL;
static struct xdg_wm_base*      fn__g_xdg_base      = NULL;

static void fn__g_registry_handle(
    void* data, 
    struct wl_registry* reg, 
    uint32_t id, 
    const char* interface, 
    uint32_t version
) {
    if(strcmp(interface, wl_compositor_interface.name) == 0) {
        fn__g_compositor = wl_registry_bind(
            reg,
            id,
            &wl_compositor_interface,
            1
        );
    } else if(strcmp(interface, wl_shm_interface.name) == 0) {
        fn__g_shm = wl_registry_bind(
            reg,
            id,
            &wl_shm_interface,
            1
        );
    } else if(strcmp(interface, wl_seat_interface.name) == 0) { 
        fn__g_seat = wl_registry_bind(
            reg,
            id,
            &wl_seat_interface,
            1
        );
    } else if(strcmp(interface, xdg_wm_base_interface.name) == 0) {
        fn__g_xdg_base = wl_registry_bind(
            reg,
            id,
            &xdg_wm_base_interface,
            1
        );
    }
}

static void fn__g_registry_remove(void* v, struct wl_registry* r, uint32_t id) {}

static const struct wl_registry_listener fn__g_registry_listener = {
    .global = fn__g_registry_handle,
    .global_remove = fn__g_registry_remove
};

static void fn__g_xdg_base_ping(void* data, struct xdg_wm_base* b, uint32_t s) {
    xdg_wm_base_pong(b, s);
}

static const struct xdg_wm_base_listener fn__g_xdg_base_listener = {
    .ping = fn__g_xdg_base_ping
};

//==============================================================================
//                          CLIENT SIDE DECORATIONS
//
// Window decorations (i.e. the close, minimize, maximize-button and border)
// are not provided by every wayland server. GNOME for instance does NOT provide
// the decorations and expects clients to draw provide them.
// 
// The following section is dedicated to just that.
//
//==============================================================================

#include <errno.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

//==============================================================================
//             PUBLIC FUNCTIONS DECLARED IN "./wayland_window.h"
//==============================================================================

bool fn__init_wayland_window() {
    fn__g_display = wl_display_connect(NULL);
    if(fn__g_display == NULL)
        return false;

    struct wl_registry* reg = wl_display_get_registry(fn__g_display);
    wl_registry_add_listener(reg, &fn__g_registry_listener, NULL);
    wl_display_roundtrip(fn__g_display);

    if(fn__g_compositor == NULL 
        || fn__g_xdg_base == NULL 
        || fn__g_seat == NULL) {
        fn__deinit_wayland_window();
        return false;
    }

    wl_seat_add_listener(fn__g_seat, &fn__g_csd_seat_listener, NULL);

    xdg_wm_base_add_listener(
        fn__g_xdg_base,
        &fn__g_xdg_base_listener,
        NULL
    );

    return true;
}

void fn__deinit_wayland_window() {
    if(fn__g_display != NULL) {
        wl_display_disconnect(fn__g_display);
        fn__g_display = NULL;
    }
}

bool fn__create_wayland_window(fn__window* window) {
    window->native.wl = wl_compositor_create_surface(
        fn__g_compositor
    );

    wl_surface_set_user_data(window->native.wl, window);

    window->xdg_surface = xdg_wm_base_get_xdg_surface(
        fn__g_xdg_base,
        window->native.wl
    );

    xdg_surface_add_listener(
        window->xdg_surface,
        &fn__g_xdg_csd_listener,
        window
    );

    window->xdg_toplevel = xdg_surface_get_toplevel(window->xdg_surface);
    xdg_toplevel_add_listener(
        window->xdg_toplevel,
        &fn__g_xdg_csd_top_listener,
        window
    );

    wl_surface_commit(window->native.wl);
    
    return true;
}

void fn__destroy_wayland_window(fn__window* window) {
}

void fn__wayland_window_set_size(
    fn__window* window,
    uint32_t width,
    uint32_t height
) {
    window->width = width;
    window->height = height;

    // The size of the window depends on the image buffer attached to it.
    // We have to resize the window decoration buffer and attach it.
    // This process is implicitly invoked by committing the surface.
    wl_surface_commit(window->native.wl);
}

void fn__wayland_window_set_title(fn__window* window, const char* title) {
    xdg_toplevel_set_title(window->xdg_toplevel, title);
}

void fn__wayland_window_set_visible(fn__window* window, bool visible) {
    window->state = visible 
    ? fn__window_state_visible 
    : fn__window_state_hidden;

    // Similar to `fn__wayland_window_set_visible`.
    wl_surface_commit(window->native.wl);
}

void fn__wayland_pump_events() {
    wl_display_dispatch(fn__g_display);
}

