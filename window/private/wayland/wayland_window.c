#include "wayland_window.h"
#include "xdg_shell.h"
#include "xdg_decorations_v1.h"
#include "../window_common.h"

#include <stdio.h>
#include <string.h>

#include <wayland-client.h>

//==============================================================================
//                             INTERNAL FUNCTIONS
//==============================================================================

static void fn__registry_handle(
    void*               data, 
    struct wl_registry* reg,
    uint32_t            id,
    const char*         interface,
    uint32_t            version
) {
    if(strcmp(interface, wl_compositor_interface.name) == 0)
        fn__g_window_context.wl.compositor = wl_registry_bind(
            reg,
            id,
            &wl_compositor_interface,
            1
        );
    else if(strcmp(interface, wl_subcompositor_interface.name) == 0)
        fn__g_window_context.wl.subcompositor = wl_registry_bind(
            reg,
            id,
            &wl_subcompositor_interface,
            1
        );
    else if(strcmp(interface, wl_shm_interface.name) == 0)
        fn__g_window_context.wl.shm = wl_registry_bind(
            reg,
            id,
            &wl_shm_interface,
            1
        );
    else if(strcmp(interface, wl_seat_interface.name) == 0)
        fn__g_window_context.wl.seat = wl_registry_bind(
            reg,
            id,
            &wl_seat_interface,
            1
        );
    else if(strcmp(interface, xdg_wm_base_interface.name) == 0)
        fn__g_window_context.wl.xdg = wl_registry_bind(
            reg,
            id,
            &xdg_wm_base_interface,
            1
        );
    else if(strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0)
        fn__g_window_context.wl.zxdg = wl_registry_bind(
            reg,
            id,
            &zxdg_decoration_manager_v1_interface,
            1
        ); 
}

static void fn__registry_remove(
    void*               data, 
    struct wl_registry* reg, 
    uint32_t            id
) {
}

static const struct wl_registry_listener fn__registry_listener = {
    .global = fn__registry_handle,
    .global_remove = fn__registry_remove
};

static void fn__ping(void* data, struct xdg_wm_base* base, uint32_t serial) {
    xdg_wm_base_pong(base, serial);
}

static const struct xdg_wm_base_listener fn__base_listener = {
    .ping = fn__ping
};

//==============================================================================
//                          CLIENT SIDE DECORATIONS
//
// Window decorations (i.e. the border and titlebar around the window's content)
// is optional in wayland. Some compositors like KDE implement them server side
// and provide the convenient zxdg_decoration interface.
// Other compositors like GNOME's Mutter don't, requiring the client to provide
// its own decorations. Those are implemented in the following section.
//
//==============================================================================

#include <errno.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

static void fn__buffer_release(void* data, struct wl_buffer* buffer) {
    wl_buffer_destroy(buffer);
}

static const struct wl_buffer_listener fn__buffer_listener = {
    .release = fn__buffer_release
};

static int fn__allocate_shm_buffer(size_t size) {
    uint8_t attempts_left = 100;
    int fd = -1;

    do {
        char name[] = "/wl_shm-XXXXXX";
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        long r = ts.tv_nsec;

        for(int it = 0; it < 6; ++it) {
            name[it] = 'A' + (r&15) + (r&16) * 2;
            r >>= 5;
        }

        --attempts_left;

        fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if(fd >= 0) {
            shm_unlink(name);
            break;
        }
    } while(attempts_left && errno == EEXIST);

    if(fd < 0)
        return -1;

    int ret;
    do{ ret = ftruncate(fd, size); } while(ret < 0 && errno == EINTR);

    if(ret < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

static struct wl_buffer* fn__decorate(fn__window* window) {
    const uint32_t stride = window->width * 4;
    const uint32_t size = stride * window->height;

    int fd = fn__allocate_shm_buffer(size);
    if(fd == -1) return NULL;

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

    struct wl_shm_pool* pool = wl_shm_create_pool(
        fn__g_window_context.wl.shm,
        fd,
        size
    );

    struct wl_buffer* buffer = wl_shm_pool_create_buffer(
        pool,
        0,
        window->width,
        window->height,
        stride,
        WL_SHM_FORMAT_XRGB8888
    );

    wl_shm_pool_destroy(pool);
    close(fd);

    memset(data, 0x2d3436, size); 
    munmap(data, size);
    wl_buffer_add_listener(buffer, &fn__buffer_listener, NULL);

    return buffer;
}

static void fn__surface_configure(
    void*               data,
    struct xdg_surface* surface,
    uint32_t            serial
) {
    xdg_surface_ack_configure(surface, serial);

    fn__window* ptr = data;
    wl_surface_attach(ptr->native.wl.meta, fn__decorate(ptr), 0, 0);
    wl_surface_commit(ptr->native.wl.meta);
}

static const struct xdg_surface_listener fn__surface_listener = {
    .configure = fn__surface_configure
};

static void fn__toplevel_configure(
    void*                   data,
    struct xdg_toplevel*    toplevel,
    int32_t                 width,
    int32_t                 height,
    struct wl_array*        states
) {
    fn__window* ptr     = data;
    bool is_resizing    = false;

    uint32_t* state;
    wl_array_for_each(state, states) {
        if(*state = XDG_TOPLEVEL_STATE_RESIZING) is_resizing = true;
    }

    if(is_resizing) {
        ptr->width = width; 
        ptr->height = height;
    }
}

static void fn__toplevel_close(
    void*                   data,
    struct xdg_toplevel*    toplevel
) {
}

static const struct xdg_toplevel_listener fn__toplevel_listener = {
    .configure  = fn__toplevel_configure,
    .close      = fn__toplevel_close
};

// ---------------------------- POINTER HANDLER --------------------------------

static void fn__pointer_enter(
    void*               data,
    struct wl_pointer*  pointer,
    uint32_t            serial,
    struct wl_surface*  surface,
    wl_fixed_t          x,
    wl_fixed_t          y
) {
}

static void fn__pointer_leave(
    void*               data,
    struct wl_pointer*  pointer,
    uint32_t            serial,
    struct wl_surface*  surface
) {
}

static void fn__pointer_motion(
    void*               data,
    struct wl_pointer*  pointer,
    uint32_t            time,
    wl_fixed_t          x,
    wl_fixed_t          y
) {
}

static void fn__pointer_button(
    void*               data,
    struct wl_pointer*  pointer,
    uint32_t            serial,
    uint32_t            time,
    uint32_t            button,
    uint32_t            state
) {
}

static void fn__pointer_axis(
    void*               data,
    struct wl_pointer*  pointer,
    uint32_t            time,
    uint32_t            axis,
    wl_fixed_t          value
) {
}

static const struct wl_pointer_listener fn__pointer_listener = {
    .enter = fn__pointer_enter,
    .leave = fn__pointer_leave,
    .motion = fn__pointer_motion,
    .button = fn__pointer_button,
    .axis = fn__pointer_axis
};

static void fn__seat_capabilities(
    void*           data,
    struct wl_seat* seat,
    uint32_t        capabilities
) {
    if(capabilities & WL_SEAT_CAPABILITY_POINTER) {
        struct wl_pointer* pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(
            pointer,
            &fn__pointer_listener,
            NULL
        );
    }
}

static void fn__seat_name(
    void*           data,
    struct wl_seat* seat,
    const char*     name
) {
}

static const struct wl_seat_listener fn__seat_listener = {
    .capabilities   = fn__seat_capabilities,
    .name           = fn__seat_name
};

// -------------------------- SUBSURFACE RENDERER ------------------------------

static const struct wl_callback_listener fn__frame_listener;

static void fn__frame_done(void* data, struct wl_callback* cb, uint32_t time) {
    printf("Frame is done.\n");

    wl_callback_destroy(cb);

    fn__window* ptr = data;
    cb = wl_surface_frame(ptr->native.wl.surface);
    wl_callback_add_listener(cb, &fn__frame_listener, data);
}

static const struct wl_callback_listener fn__frame_listener = {
    .done = fn__frame_done
};

static struct wl_buffer* fn__draw_frame(fn__window* window) {
    if(window->width < 10 || window->height < 10)
        return NULL;

    const uint32_t width = window->width - 10;
    const uint32_t height = window->height - 10;
    
    const uint32_t stride = width * 4;
    const uint32_t size = stride * window->height;

    int fd = fn__allocate_shm_buffer(size);
    if(fd == -1) return NULL;

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

    struct wl_shm_pool* pool = wl_shm_create_pool(
        fn__g_window_context.wl.shm,
        fd,
        size
    );

    struct wl_buffer* buffer = wl_shm_pool_create_buffer(
        pool,
        0,
        window->width,
        window->height,
        stride,
        WL_SHM_FORMAT_XRGB8888
    );

    wl_shm_pool_destroy(pool);
    close(fd);

    memset(data, 0x636e72, size); 
    munmap(data, size);
    wl_buffer_add_listener(buffer, &fn__buffer_listener, NULL);

    return buffer;
}

//==============================================================================
//             PUBLIC FUNCTIONS DECLARED IN "./wayland_window.h"
//==============================================================================

bool fn__init_wayland_window() {
    fn__g_window_context.wl.display = wl_display_connect(NULL);
    if(!fn__g_window_context.wl.display) return false;

    struct wl_registry* reg = wl_display_get_registry(
        fn__g_window_context.wl.display
    );

    wl_registry_add_listener(reg, &fn__registry_listener, NULL);
    wl_display_roundtrip(fn__g_window_context.wl.display);

    if(fn__g_window_context.wl.compositor == NULL
        || fn__g_window_context.wl.subcompositor == NULL
        || fn__g_window_context.wl.shm == NULL
        || fn__g_window_context.wl.seat == NULL
        || fn__g_window_context.wl.xdg == NULL) {
        fn__deinit_wayland_window();
        return false;
    }

    xdg_wm_base_add_listener(
        fn__g_window_context.wl.xdg,
        &fn__base_listener,
        NULL
    );

    wl_seat_add_listener(
        fn__g_window_context.wl.seat,
        &fn__seat_listener,
        NULL
    );

    return true;
}

void fn__deinit_wayland_window() {
    if(fn__g_window_context.wl.xdg != NULL) {
        xdg_wm_base_destroy(fn__g_window_context.wl.xdg);
        fn__g_window_context.wl.xdg = NULL;
    }

    if(fn__g_window_context.wl.display != NULL) {
        wl_display_disconnect(fn__g_window_context.wl.display);
        fn__g_window_context.wl.display = NULL;
    }
}

bool fn__create_wayland_window(fn__window* window) {
    window->native.wl.surface = wl_compositor_create_surface(
        fn__g_window_context.wl.compositor
    );

    window->native.wl.meta = wl_compositor_create_surface(
        fn__g_window_context.wl.compositor
    );

    window->native.wl.subsurface = wl_subcompositor_get_subsurface(
        fn__g_window_context.wl.subcompositor,
        window->native.wl.surface,
        window->native.wl.meta
    );

    wl_subsurface_set_desync(window->native.wl.subsurface);
    wl_subsurface_set_position(window->native.wl.subsurface, 5, 5);

    window->native.wl.xdg_surface = xdg_wm_base_get_xdg_surface(
        fn__g_window_context.wl.xdg,
        window->native.wl.meta
    );

    window->native.wl.xdg_toplevel = xdg_surface_get_toplevel(
        window->native.wl.xdg_surface
    );

    xdg_surface_add_listener(
        window->native.wl.xdg_surface,
        &fn__surface_listener,
        window
    );

    xdg_toplevel_add_listener(
        window->native.wl.xdg_toplevel,
        &fn__toplevel_listener,
        window
    );

    wl_surface_commit(window->native.wl.meta);

    return true;
}

void fn__destroy_wayland_window(fn__window* window) {
    wl_surface_destroy(window->native.wl.surface);
    window->native.wl.surface = NULL;

    wl_surface_destroy(window->native.wl.meta);
    window->native.wl.meta = NULL;

    wl_subsurface_destroy(window->native.wl.subsurface);
    window->native.wl.subsurface = NULL;

    xdg_surface_destroy(window->native.wl.xdg_surface);
    window->native.wl.xdg_surface = NULL;

    xdg_toplevel_destroy(window->native.wl.xdg_toplevel);
    window->native.wl.xdg_toplevel = NULL;
}

void fn__wayland_window_set_size(
    fn__window* window,
    uint32_t width,
    uint32_t height
) {
    window->width = width;
    window->height = height;
    wl_surface_commit(window->native.wl.meta);
}

void fn__wayland_window_set_title(fn__window* window, const char* title) {
    xdg_toplevel_set_title(window->native.wl.xdg_toplevel, title);
}

void fn__wayland_window_set_visible(fn__window* window, bool visible) {
}

void fn__wayland_pump_events() {
    wl_display_dispatch(fn__g_window_context.wl.display);
}
