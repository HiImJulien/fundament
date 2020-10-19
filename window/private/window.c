#include <fundament/window.h>
#include "window_common.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <inttypes.h>

//==============================================================================
//                                INTERNAL API
//==============================================================================

#if defined(_WIN32)
    #include "win32/win32_window.h"
#elif defined(__APPLE__)
    #include "cocoa/cocoa_window.h"
#endif

//
// Internal utility, which return a pointer to the corresponding window,
// or NULL, if the handle is invalid.
//
static inline struct fn__window* fn__get_window(struct fn_window window) {
    uint32_t index;
    if(!fn_check_handle(&fn__g_window_context.window_pool, window.id, &index))
        return NULL; 

    struct fn__window* ptr = &fn__g_window_context.windows[index]; 
    return ptr->state != fn__window_state_closed ? ptr : NULL;
} 

//==============================================================================
//                                 PUBLIC API
//==============================================================================

bool fn_init_window() {
    if (fn__g_window_context.initialized == true)
        return true;

    fn__g_window_context.initialized = true;
    fn_initialize_handle_pool(
        &fn__g_window_context.window_pool,
        FN_WINDOW_CAPACITY
    );

    return fn__init_imp_window();
}

void fn_deinit_window() {
    if (!fn__g_window_context.initialized)
        return;

    for(size_t it = 0; it < FN_WINDOW_CAPACITY; ++it) {
        struct fn__window* ptr = &fn__g_window_context.windows[it];

        // Free the memory allocated by the title.
        free((char*) ptr->title); 
        ptr->title = NULL;

        if(ptr->state)
            fn__destroy_imp_window(ptr);

        *ptr = (struct fn__window) { 0, };
    }

    free(fn__g_window_context.events);
    fn__g_window_context = (struct fn__window_context) { 0, };

    fn__deinit_imp_window();
}

struct fn_window fn_create_window() {
    uint32_t handle, index; 
    if(!fn_alloc_handle(&fn__g_window_context.window_pool, &handle, &index))
        return (struct fn_window) { 0 };

    struct fn__window* ptr = &fn__g_window_context.windows[index];
    if(!fn__create_imp_window(ptr))
        return (struct fn_window) { 0 };

    ptr->state = fn__window_state_hidden;

    return (struct fn_window) { handle };
}

void fn_destroy_window(struct fn_window window) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;

    fn__destroy_imp_window(ptr); 

    ptr->state = fn__window_state_closed;
    fn_dealloc_handle(
        &fn__g_window_context.window_pool,
        window.id
    );
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
    return ptr ? (ptr->state > fn__window_state_hidden) : false;
}

bool fn_window_focused(struct fn_window window) {
    struct fn__window* ptr = fn__get_window(window);
    return ptr ? ptr->focused : false;
}

fn_native_window_handle_t fn_window_handle(struct fn_window window) {
    struct fn__window* ptr = fn__get_window(window);
    return ptr ? ptr->native : NULL;
}

void fn_window_set_width(
    struct fn_window window,
    uint32_t width
) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;

    fn__imp_window_set_size(ptr, width, ptr->height);
}

void fn_window_set_height(
    struct fn_window window,
    uint32_t height
) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;

    fn__imp_window_set_size(ptr, ptr->width, height);
}

void fn_window_set_size(
    struct fn_window window,
    uint32_t width,
    uint32_t height
) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;

    fn__imp_window_set_size(ptr, width, height);
}

void fn_window_set_title(
    struct fn_window window,
    const char* title
) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;

    // To this day, I am baffled by the fact, that `strdup` is not function
    // provided by ISO C until C2X.
    if(ptr->title)
        free((char*) ptr->title);

    const size_t len = strlen(title);
    ptr->title = malloc(sizeof(char) * (len + 1));

#if defined(__STDC_LIB_EXT1__)
    strncpy_s(
        (char*) ptr->title,
        len + 1,
        title,
        len + 1
    );
#else
    strncpy(
        (char*) ptr->title,
        title,
        len + 1
    );
#endif

    fn__imp_window_set_title(ptr, title);
}

void fn_window_set_visible(
    struct fn_window window,
    bool visible
) {
    struct fn__window* ptr = fn__get_window(window);
    if(!ptr)
        return;

    fn__imp_window_set_visible(ptr, visible);
    ptr->state = visible
        ? fn__window_state_visible
        : fn__window_state_hidden;
}

void fn_poll_events(struct fn_event* ev) {
    fn__imp_pump_events();
    fn__pop_event(ev);
}

