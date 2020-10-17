#include <fundament/window.h>
#include "window_common.h"

//==============================================================================
//                                INTERNAL API
//==============================================================================

//
// Internal utility, which return a pointer to the corresponding window,
// or NULL, if the handle is invalid.
//
static inline struct fn__window* fn__get_window(struct fn_window window) {
    uint32_t index;
    if(!fn_check_handle(&fn__g_window_context.window_pool, window.id, &index))
        return NULL; 

    struct fn__window* ptr = &fn__g_window_context.windows[index]; 
    return ptr->state ? ptr : NULL;
} 

//==============================================================================
//                                 PUBLIC API
//==============================================================================

bool fn_init_window() {
    return true;
}

struct fn_window fn_create_window() {
    uint32_t handle, index; 
    if(!fn_alloc_handle(&fn__g_window_context.window_pool, &handle, &index))
        return (struct fn_window) { 0 };

    // TODO: Create the window.

    return (struct fn_window) { handle };
}

void fn_destroy_window(struct fn_window window) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;

    // TODO: Destroy the window.
}

uint32_t fn_window_width(struct fn_window window) {
    struct fn__window* ptr = fn__get_window(window); 
    return ptr ? ptr->width : 0;
}

uint32_t fn_window_height(struct fn_window window) {
    struct fn__window* ptr = fn__get_window(window);
    return ptr ? ptr->height : 0;
}

const char* fn_window_title(struct fn_window window) {
    struct fn__window* ptr = fn__get_window(window);
    return ptr ? ptr->title : NULL;
}

bool fn_window_visible(struct fn_window window) {
    struct fn__window* ptr = fn__get_window(window);
    return ptr ? ptr->state : false;
}

fn_native_window_handle_t fn_window_handle(struct fn_window window) {
    struct fn__window* ptr = fn__get_window(window);
    // TODO: Retrieve native handle
    return NULL;
}

void fn_window_set_width(
    struct fn_window window,
    uint32_t width
) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;

    // TODO: Set width
}

void fn_window_set_height(
    struct fn_window window,
    uint32_t height
) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;

    // TODO: Set height
}

void fn_window_set_size(
    struct fn_window window,
    uint32_t width,
    uint32_t height
) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;

    // TODO: Set size
}

void fn_window_set_title(
    struct fn_window window,
    const char* title
) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;

    // TODO: Set title
}

void fn_window_set_visible(
    struct fn_window window,
    bool visible
) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;
}

