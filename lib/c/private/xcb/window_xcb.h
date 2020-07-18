#ifndef FUNDAMENT_WINDOW_XCB_H
#define FUNDAMENT_WINDOW_XCB_H

//=============================================================================
// This module implements the internal functions used to create and manage
// windows for the xcb system.
//=============================================================================

#include <fundament/window.h>

//
// Sets up an Xlib and xcb connection, queries for XInput support and retrieves
// extended window manager hints as atoms.
//
void fn__imp_init_window_context();

//
// Creates a new window, enables XInput events for it, when applicable,
// and sets the extended window manager hints.
//
fn_native_window_handle_t fn__imp_create_window(uint32_t index);

//
// Destroys the given window.
//
void fn__imp_destroy_window(fn_native_window_handle_t handle);

//
// Sets the size of the given window.
//
void fn__imp_window_set_size(
    fn_native_window_handle_t handle, 
    uint32_t width, 
    uint32_t height
);

//
// Sets the title of the given window.
//
void fn__imp_window_set_title(
    fn_native_window_handle_t handle, 
    const char* title
);

//
// Sets the visibility of the given window.
//
void fn__imp_window_set_visibility(
    fn_native_window_handle_t handle, 
    bool visible
);

//
// Polls the next event from xcb's event queue.
//
void fn__imp_window_poll_events();

#endif  // FUNDAMENT_WINDOW_XCB_H
