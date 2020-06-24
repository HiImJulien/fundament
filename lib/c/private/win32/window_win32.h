#ifndef FUNDAMENT_WINDOW_WIN32_H
#define FUNDAMENT_WINDOW_WIN32_H

//==============================================================================
// This module implements the internal functions used to create and manage
// windows for the Win32 system.
//==============================================================================

#include <fundament/window.h>

#include <Windows.h>

//
// Registers the window class.
//
void fn__imp_init_window_context();

//
// Creates a new window using 'CreateWindowExA' and 
// sets the window index as 'user data' of the window.
//
fn_native_window_handle_t fn__imp_create_window(uint32_t index);

//
// Destroys the window using 'DestroyWindow'.
//
void fn__imp_destroy_window(fn_native_window_handle_t handle);

//
// Sets the client size of the window using 'SetWindowPosA'.
//
void fn__imp_window_set_size(
    fn_native_window_handle_t handle, 
    uint32_t width, 
    uint32_t height
);

//
// Sets the window's title using 'SetWindowTextA'.
//
void fn__imp_window_set_title(
    fn_native_window_handle_t handle, 
    const char* title
);

//
// Sets the window's visiblity.
//
void fn__imp_window_set_visibility(
    fn_native_window_handle_t handle,
    bool visible
);

//
// Polls events using 'PeekMessageA'.
//
void fn__imp_window_poll_events();

//
// Callback used to handle events.
//
LRESULT CALLBACK fn__imp_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif  // FUNDAMENT_WINDOW_WIN32_H