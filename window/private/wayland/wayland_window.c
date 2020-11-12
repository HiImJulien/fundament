#include "wayland_window.h"
#include "xdg-shell.h"
#include "wayland_decorations.h"
#include "../window_common.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <wayland-client.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

//==============================================================================
//                             INTERNAL FUNCTIONS
//==============================================================================

static struct wl_display*       fn__g_display       = NULL;
static struct wl_registry*      fn__g_registry      = NULL;
static struct wl_compositor*    fn__g_compositor    = NULL;
static struct wl_shm*           fn__g_shm           = NULL;
static struct wl_seat*          fn__g_seat          = NULL;
static struct wl_keyboard*      fn__g_keyboard      = NULL;
static struct wl_pointer*       fn__g_pointer       = NULL;
static struct xdg_wm_base*      fn__g_xdg_base      = NULL;

// Stores the window, in which the mouse currently is inside.
static struct fn__window*       fn__g_mouse_window  = NULL;

static const uint32_t           fn__g_border_color  = 0x2d3436;

static void fn__g_registry_handler(
    void* data,
    struct wl_registry* registry,
    uint32_t id,
    const char* interface,
    uint32_t version
) {
    if(strcmp(interface, wl_compositor_interface.name) == 0)
        fn__g_compositor = wl_registry_bind(
            registry,
            id,
            &wl_compositor_interface,
            1
        );
    else if(strcmp(interface, wl_shm_interface.name) == 0)
        fn__g_shm = wl_registry_bind(
            registry,
            id,
            &wl_shm_interface,
            1
        );
    else if(strcmp(interface, wl_seat_interface.name) == 0)
        fn__g_seat = wl_registry_bind(
            registry,
            id,
            &wl_seat_interface,
            5
        );
    else if(strcmp(interface, xdg_wm_base_interface.name) == 0)
        fn__g_xdg_base = wl_registry_bind(
            registry,
            id,
            &xdg_wm_base_interface,
            1
        );
}

static void fn__g_registry_remover(
    void* data,
    struct wl_registry* registry,
    uint32_t id
) {
}

static const struct wl_registry_listener fn__g_registry_listener = {
    .global         = fn__g_registry_handler,
    .global_remove  = fn__g_registry_remover
};

static void fn__g_xdg_base_ping(
    void* data, 
    struct xdg_wm_base* xdg_base, 
    uint32_t serial
) {
    xdg_wm_base_pong(xdg_base, serial);
}

static const struct xdg_wm_base_listener fn__g_xdg_base_listener = {
    .ping = fn__g_xdg_base_ping
};

static struct wl_buffer* fn__g_draw_decorations(fn__window* window);

static void fn__g_xdg_configure(
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

static const struct xdg_surface_listener fn__g_xdg_surface_listener = {
    .configure = fn__g_xdg_configure
};

static void fn__g_xdg_toplevel_configure(
    void* data,
    struct xdg_toplevel* toplevel,
    int32_t width,
    int32_t height,
    struct wl_array* states
) {
}

static void fn__g_xdg_toplevel_close(
    void* data,
    struct xdg_toplevel* toplevel
) {
    fn__window* ptr = (fn__window*) data;
    fn__notify_window_closed(ptr);
}

static const struct xdg_toplevel_listener fn__g_xdg_toplevel_listener = {
    .configure  = fn__g_xdg_toplevel_configure,
    .close      = fn__g_xdg_toplevel_close 
};

static void fn__g_buffer_release(void* data, struct wl_buffer* buffer) {
    wl_buffer_destroy(buffer);
}

static const struct wl_buffer_listener fn__g_buffer_listener = {
    .release = fn__g_buffer_release
};

static void fn__g_random_file_name(char* buffer) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long r = ts.tv_nsec;
    for(int it = 0; it < 6; ++it) {
        buffer[it] = 'A' + (r&15) + (r&16) * 2;
        r >>= 5;
    }
}

static int fn__g_create_shm(size_t size) {
    uint8_t attempts = 100;
    do {
        char name[] = "/wl_shm-XXXXXX";
        fn__g_random_file_name(name + sizeof(name) - 7);
        --attempts;

        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if(fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while(attempts && errno == EEXIST);
    return -1;
}

static int fn__g_allocate_shm(size_t size) {
    int fd = fn__g_create_shm(size);
    if(fd < 0)
        return -1;

    int ret;
    do {
        ret = ftruncate(fd, size);
    } while(ret < 0 && errno == EINTR);

    if(ret < 0) {
        close(fd);
        return -1;
    }
        
    return fd;
}

static struct wl_buffer* fn__g_draw_decorations(fn__window* window) {
    uint32_t stride = window->width * 4;
    uint32_t size = stride * window->height;

    int fd = fn__g_allocate_shm(size);
    if(fd == -1)
        return NULL;

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
        window->width,
        window->height,
        stride,
        WL_SHM_FORMAT_XRGB8888
    );

    wl_shm_pool_destroy(pool);
    close(fd);

    fn__decorate_wayland_window(window, data);

    munmap(data, size);
    wl_buffer_add_listener(buffer, &fn__g_buffer_listener, NULL);
    return buffer;
}

static void fn__g_pointer_enter(
    void* data,
    struct wl_pointer* wl_pointer,
    uint32_t serial,
    struct wl_surface* surface,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y
) {
    fn__window* ptr = wl_surface_get_user_data(surface);
    fn__g_mouse_window = ptr; 
}

static void fn__g_pointer_leave(
    void* data,
    struct wl_pointer* wl_pointer,
    uint32_t serial,
    struct wl_surface* surface
) {
    fn__g_mouse_window = NULL;
}

static void fn__g_pointer_move(
    void* data,
    struct wl_pointer* wl_pointer,
    uint32_t time,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y
) {
}

static void fn__g_pointer_button(
    void* data,
    struct wl_pointer* wl_pointer,
    uint32_t serial,
    uint32_t time,
    uint32_t button,
    uint32_t state
) {
    if(fn__g_mouse_window) {
        xdg_toplevel_move(fn__g_mouse_window->xdg_toplevel, fn__g_seat, serial);
    }
}

static void fn__g_pointer_axis(
    void* data,
    struct wl_pointer* wl_pointer,
    uint32_t time,
    uint32_t axis,
    wl_fixed_t value
) {
}

static void fn__g_pointer_axis_source(
    void* data,
    struct wl_pointer* wl_pointer,
    uint32_t axis_source
) {
}

static void fn__g_pointer_axis_stop(
    void* data,
    struct wl_pointer* wl_pointer,
    uint32_t time,
    uint32_t axis
) {
}

static void fn__g_pointer_axis_discrete(
    void* data,
    struct wl_pointer* wl_pointer,
    uint32_t axis,
    int32_t discrete
) {
}

static void fn__g_pointer_frame(
    void* data,
    struct wl_pointer* wl_pointer
) {
    
}

static const struct wl_pointer_listener fn__g_pointer_listener = {
    .enter = fn__g_pointer_enter,
    .leave = fn__g_pointer_leave,
    .motion = fn__g_pointer_move,
    .button = fn__g_pointer_button,
    .axis = fn__g_pointer_axis,
    .frame = fn__g_pointer_frame,
    .axis_source = fn__g_pointer_axis_source,
    .axis_stop = fn__g_pointer_axis_stop,
    .axis_discrete = fn__g_pointer_axis_discrete
};

static void fn__g_seat_capabilities(
    void* data, 
    struct wl_seat* seat, 
    uint32_t caps
) {
    if(caps & WL_SEAT_CAPABILITY_POINTER) {
        fn__g_pointer = wl_seat_get_pointer(fn__g_seat);
        wl_pointer_add_listener(fn__g_pointer, &fn__g_pointer_listener, NULL);
    } 
}

static void fn__g_seat_name(
    void* data,
    struct wl_seat* seat,
    const char* name
) {
    printf("Seat name: %s\n", name);
}

static struct wl_seat_listener seat_listener = {
    .capabilities = fn__g_seat_capabilities,
    .name = fn__g_seat_name
};

//==============================================================================
//             PUBLIC FUNCTIONS DECLARED IN "./wayland_window.h"
//==============================================================================

bool fn__init_wayland_window() {
    fn__g_display = wl_display_connect(NULL);
    if(fn__g_display == NULL)
        return false;

    fn__g_registry = wl_display_get_registry(fn__g_display);
    wl_registry_add_listener(fn__g_registry, &fn__g_registry_listener, NULL);
    wl_display_roundtrip(fn__g_display);

    wl_seat_add_listener(fn__g_seat, &seat_listener, NULL);

    xdg_wm_base_add_listener(
        fn__g_xdg_base,
        &fn__g_xdg_base_listener,
        NULL
    );

    fn__init_wayland_decorations();

    return true;
}

void fn__deinit_wayland_window() {
    if(fn__g_display) {
        wl_display_disconnect(fn__g_display);
        fn__g_display = NULL;
    }

    fn__deinit_wayland_decorations();
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
        &fn__g_xdg_surface_listener,
        window
    );

    window->xdg_toplevel = xdg_surface_get_toplevel(window->xdg_surface);

    xdg_toplevel_add_listener(
        window->xdg_toplevel,
        &fn__g_xdg_toplevel_listener,
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
    wl_surface_commit(window->native.wl);
}

void fn__wayland_window_set_title(
    fn__window* window,
    const char* title
) {
    xdg_toplevel_set_title(window->xdg_toplevel, title);
    xdg_toplevel_set_app_id(window->xdg_toplevel, title);
    wl_surface_commit(window->native.wl);
}

void fn__wayland_window_set_visible(
    fn__window* window,
    bool visible
) {
}

void fn__wayland_pump_events() {
    wl_display_dispatch(fn__g_display);
}


