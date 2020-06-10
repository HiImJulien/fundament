#ifndef FUNDAMENT_WINDOW_APPKIT_H
#define FUNDAMENT_WINDOW_APPKIT_H

#include <fundament/window.h>

//
// Sets up the process for 'GUI' usage,
// by setting the activation policy and calling 'finishLaunching'
// on the shared NSApplication instance.
//
void fn__imp_init_window_context();

//
// Creates a new instance of 'NSWindow' and 'fn_window_delegate',
// sets the latter as the priors delegate.
//
fn_native_window_handle_t fn__imp_create_window(uint32_t index);

//
// Releases the window ands its delegate instance.
//
void fn__imp_destroy_window(fn_native_window_handle_t handle);

//
// Sets the window's content size.
//
void fn__imp_window_set_size(
    fn_native_window_handle_t handle,
    uint32_t width,
    uint32_t height
);

//
// Sets the window's title.
//
void fn__imp_window_set_title(
    fn_native_window_handle_t handle,
    const char* title
);

//
// Sets the visibility of the window.
//
void fn__imp_window_set_visibility(
    fn_native_window_handle_t handle,
    bool visible
);

//
// Polls the next event from the underlying system.
//
void fn__imp_window_poll_events();


#endif  // FUNDAMENT_WINDOW_APPKIT_H
