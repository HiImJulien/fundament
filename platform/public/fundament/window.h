#ifndef FUNDAMENT_WINDOW_H
#define FUNDAMENT_WINDOW_H

//==============================================================================
//                                   WINDOW
//
// Window, which can serve as a render target.
//
//==============================================================================

#include <fundament/types.h>

#include <stdbool.h>
#include <stdint.h>

struct fn_window{ uint32_t id; };

//
// Initializes the window module.
//
bool fn_init_window();

//
// Creates a new window. 
//
struct fn_window fn_create_window();

//
// Destroys a window.
//
void fn_destroy_window(struct fn_window window);

//
// Returns the client width of the window or 0,
// if the given handle is invalid.
//
uint32_t fn_window_width(struct fn_window window);

//
// Returns the client height of the window or 0,
// if the given handle is invalid.
//
uint32_t fn_window_height(struct fn_window window);

//
// Returns the title of the window.
//
// Returns NULL, if either the handle is invalid or if the window
// had not title set.
//
const char* fn_window_title(struct fn_window window);

//
// Returns true if the window is set to be visible,
// otherwise returns false.
//
bool fn_window_visible(struct fn_window window);

//
// Returns the native handle of the window.
//
// Returns NULL (or 0 on linux), if the handle is invalid.
//
fn_native_window_handle_t fn_window_handle(struct fn_window window);

//
// Sets the client width of the window.
//
// Does nothing if the handle is invalid.
//
void fn_window_set_width(
    struct fn_window window,
    uint32_t width
);

//
// Sets the client height of the window.
//
// Does nothing if the handle is invalid.
//
void fn_window_set_height(
    struct fn_window window,
    uint32_t height
);

//
// Sets the client size of the window.
//
// Does nothing if the handle is invalid.
//
void fn_window_set_size(
    struct fn_window window,
    uint32_t width,
    uint32_t height
);

//
// Sets the title of the window.
//
// Does nothing if the handle is invalid.
//
void fn_window_set_title(
    struct fn_window window,
    const char* title
);

//
// Sets whether the window is to be seen or not.
//
// Does nothing if the handle is invalid.
//
void fn_window_set_visible(
    struct fn_window window,
    bool visible
);

#endif  // FUNDAMENT_WINDOW_H

