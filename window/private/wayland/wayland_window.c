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

static const uint32_t fn__g_csd_bg = 0x2d3436;
static const uint32_t fn__g_csd_fg = 0x636e72;
static const uint32_t fn__g_csd_cl = 0xd63031;

static void fn__g_buffer_release(void* data, struct wl_buffer* buffer) {
    wl_buffer_destroy(buffer);
}

static const struct wl_buffer_listener fn__g_buffer_listener = {
    .release = fn__g_buffer_release
};

static int fn__g_create_shm_buffer(size_t size) {
    uint8_t attempts_left = 100;
    int fd = -1;

    do {
        // Create a random filename.
        char name[] = "/wl_shm-XXXXXX";
        struct timespec ts; 
        clock_gettime(CLOCK_REALTIME, &ts);
        long r = ts.tv_nsec;
        for(int it = 0; it < 6; ++it) {
            name[it] = 'A' + (r&15) + (r&16) * 2;
            r >>= 5;
        }

        --attempts_left;

        // Attempt to open a shared memory file. 
        fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if(fd >= 0) {
            shm_unlink(name);
            break;
        }
    } while(attempts_left && errno == EEXIST);

    if(fd < 0)
        return -1;

    // Ensure size
    int ret;
    do{ ret = ftruncate(fd, size); } while(ret < 0 && errno == EINTR);

    if(ret < 0) {
        close(fd);
        return -1;
    } 

    return fd;
}

static void fn__g_draw_rectangle(
    uint32_t* data,
    uint32_t data_width,
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height,
    uint32_t color
) {
    for(uint32_t it = 0; it < height; ++it)
        for(uint32_t jt = 0; jt < width; ++jt)
            data[(it + y) * data_width + jt + x] = color;
}

static struct wl_buffer* fn__g_draw_decorations(fn__window* window) {
    // The window has a title bar, which is 10 pixel in height, additionally
    // there is a border around the window, which is one pixel thick.
    const uint32_t border_width = 1;
    const uint32_t title_height = 20;

    const uint32_t width = window->width + 2 * border_width;
    const uint32_t height = window->height + border_width + title_height;
    uint32_t stride = width * 4; 
    uint32_t size = stride * height;

    int fd = fn__g_create_shm_buffer(size);
    if(fd == -1) {
        return NULL;
        printf("Failed to create shm buffer.");
    }

    uint32_t* data = mmap(
        NULL,   
        size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );

    if(data == MAP_FAILED) {
        close(fd);
        return NULL;
    }

    struct wl_shm_pool* pool = wl_shm_create_pool(fn__g_shm, fd, size);
    struct wl_buffer*   buffer = wl_shm_pool_create_buffer(
        pool,
        0,
        width,
        height,
        stride,
        WL_SHM_FORMAT_XRGB8888
    );

    wl_shm_pool_destroy(pool);
    close(fd);

    memset(data, fn__g_csd_bg, size);

    // The control buttons are, for the time being, just rectangles with a 
    // different color.
    const uint32_t margin_right = 10;
    const uint32_t cb_size = 20;
    uint32_t position_right = width - cb_size - margin_right;
    fn__g_draw_rectangle(
        data, 
        width, 
        position_right,
        0,
        cb_size, 
        cb_size,
        fn__g_csd_cl
    );

    position_right -= (cb_size + margin_right);
    fn__g_draw_rectangle(
        data, 
        width, 
        position_right,
        0,
        cb_size, 
        cb_size,
        fn__g_csd_fg
    );
    
    position_right -= (cb_size + margin_right);
    fn__g_draw_rectangle(
        data, 
        width, 
        position_right,
        0,
        cb_size, 
        cb_size,
        fn__g_csd_fg
    );

    munmap(data, size);
    wl_buffer_add_listener(buffer, &fn__g_buffer_listener, NULL);

    return buffer;
}

static void fn__g_xdg_csd_configure(
    void* data,
    struct xdg_surface* surface,
    uint32_t serial
) {
    xdg_surface_ack_configure(surface, serial);

    fn__window* ptr = (fn__window*) data;
    struct wl_buffer* buffer = fn__g_draw_decorations(ptr);
    wl_surface_attach(ptr->native.wl, buffer, 0, 0);
    wl_surface_commit(ptr->native.wl);
}

static const struct xdg_surface_listener fn__g_xdg_csd_listener = {
    .configure = fn__g_xdg_csd_configure
};

static void fn__g_xdg_csd_top_conf(
    void* data,
    struct xdg_toplevel* toplevel,
    int32_t width,
    int32_t height,
    struct wl_array* states
) {
}

static void fn__g_xdg_csd_top_close(void* data, struct xdg_toplevel* toplevel) {
    fn__window* ptr = (fn__window*) data;
    fn__notify_window_closed(ptr);
}

static const struct xdg_toplevel_listener fn__g_xdg_csd_top_listener = {
    .configure = fn__g_xdg_csd_top_conf,
    .close = fn__g_xdg_csd_top_close
};

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

    if(fn__g_compositor == NULL || fn__g_xdg_base == NULL) {
        fn__deinit_wayland_window();
        return false;
    }

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

