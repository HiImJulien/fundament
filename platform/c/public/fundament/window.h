#ifndef FUNDAMENT_WINDOW_H
#define FUNDAMENT_WINDOW_H

//==============================================================================
// This module provides functionality to create and manage windows,
// which can be used as a render target.
//==============================================================================

#include <fundament/api.h>

#include <stdbool.h>
#include <stdint.h>

struct fn_event;
struct fn_window {
    uint32_t id;
};

//
// Initializes the windowing module.
//
API void fn_init_window_module();

//
// Deinitializes the windowing module,
// releasing and invalidating all its resources.
//
API void fn_deinit_window_module();

//
// Creates a new window and returns a handle to it.
//
// If creation fails due to an error, the handle returned
// is invalid.
//
API struct fn_window fn_create_window();

//
// Destroys a window.
//
// Does nothing, if the handle is invalid or either the window
// is already destroyed.
//
API void fn_destroy_window(struct fn_window window);

//
// Returns the current client width of the window.
//
// Returns 0, if the handle is invalid or the window is already
// destroyed.
//
API uint32_t fn_window_width(struct fn_window window);

//
// Returns the current client height of the window.
//
// Returns 0, if the handle is invalid or the window is already
// destroyed.
//
API uint32_t fn_window_height(struct fn_window window);

//
// Sets the current client width of the window.
//
// Does nothing, if the handle is invalid or either the window
// is already destroyed.
//
API void fn_window_set_width(struct fn_window window, uint32_t width);

//
// Sets the current client height of the window.
//
// Does nothing, if the handle is invalid or either the window
// is already destroyed.
//
API void fn_window_set_height(struct fn_window window, uint32_t height);

//
// Returns the current title of the window.
//
// Returns NULL, if the handle is invalid or the window is already
// destroyed.
// Returns also NULL, when no title was yet assigned to the window.
//
// Note, that the memory is owned by the framework.
//
API const char* fn_window_title(struct fn_window window);

//
// Sets the current title of the window.
//
// Does nothing, if the handle is invalid or either the window
// is already destroyed.
//
// Note, that the function creates a copy of the string.
//
API void fn_window_set_title(struct fn_window window, const char* title);

//
// Returns the currently set visibility of the window.,
//
API bool fn_window_visible(struct fn_window window);

//
// Sets the visibility of the window.
//
// Does nothing, if the handle is invalid or either the window
// is already destroyed.
//
API void fn_window_set_visibility(struct fn_window window, bool visible);

//
// Polls the next event from the internal event queue.
//
API void fn_poll_events(struct fn_event* ev);

//==============================================================================
// Typedefs the native window handles.
//==============================================================================

#if defined(_WIN32)
typedef struct HWND__* fn_native_window_handle_t;
#elif defined(__APPLE__) && defined(__OBJC__)
@class NSWindow;
typedef NSWindow* fn_native_window_handle_t;
#elif defined(__APPLE__) && !defined(__OBJC__)
typedef void* fn_native_window_handle_t;
#elif defined(__linux__)
typedef uint32_t fn_native_window_handle_t;
#else
    #error "Unknown Target!"
#endif

//
// Returns the native window handle of the window.
//
// Returns 0 or NULL, if the passed window is deemed invalid.
//
API fn_native_window_handle_t fn_window_handle(struct fn_window window);

#endif  // FUNDAMENT_WINDOW_H
