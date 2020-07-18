#ifndef FUNDAMENT_WINDOW_XCB_H
#define FUNDAMENT_WINDOW_XCB_H

//=============================================================================
// This module implements the internal functions used to create and manage
// windows for the xcb system.
//=============================================================================

#include <fundament/window.h>

//
// Initiates the xcb connection.
//
void fn__imp_init_window_context();

fn_native_window_handle_t fn__imp_create_window(uint32_t index);

void fn__imp_destroy_window(fn_native_window_handle_t handle);

void fn__imp_window_set_size(
    fn_native_window_handle_t handle, uint32_t width, uint32_t height
);

void fn__imp_window_set_title(
    fn_native_window_handle_t handle, const char* title
);

void fn__imp_window_set_visibility(
    fn_native_window_handle_t handle, bool visible
);

void fn__imp_window_poll_events();

#endif  // FUNDAMENT_WINDOW_XCB_H
