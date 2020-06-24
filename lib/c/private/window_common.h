#ifndef FUNDAMENT_WINDOW_COMMON_H
#define FUNDAMENT_WINDOW_COMMON_H

//==============================================================================
// This files declares functions and defines structures, that are used commonly
// across window implementations.
//==============================================================================

#include <fundament/window.h>
#include <fundament/input.h>

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

//==============================================================================
// Includes the appropriate implementation based on the compile target.
// This include reside here in order to avoid code duplication,
// as the function 'fn__imp_deinit_window_context' requires 
// access to the 'fn__imp_destroy_window' function, while the other functions
// are needed in the file 'window.c'.
//==============================================================================

#if defined(_WIN32)
    #include "win32/window_win32.h"
#elif defined(__APPLE__)
    #include "AppKit/window_AppKit.h"
#elif defined(__linux__)
    #include "xcb/window_xcb.h"
#endif

//
// Internal representation of a window and its state.
//
// Window handles or comprised of two parts.
// The upper 8 bit specify the handles "generation",
// which can be used to prevent handle collision with 
// old handles.
//
struct fn__window {
    fn_native_window_handle_t   handle;
    uint32_t                    width;
    uint32_t                    height;
    const char*                 title;
    bool                        visible;
    bool                        focused;
};

//
// Encapsulates data shared across the windowing API.
//
struct fn__window_context {
    struct fn__window*      windows;
    size_t                  windows_capacity;
    size_t                  windows_size;

    struct fn_event*        events;
    size_t                  events_capacity;
    size_t                  events_size;
    size_t                  events_tail;

    struct fn_window        focused_window;
};

//
// Stores the data shared across the windowing API.
//
extern struct fn__window_context fn__g_window_context;

//
// Initializes the global window context.
// 
void fn__init_window_context();

//
// Deinitializes the global window context.
//
// Destroys every window that might remain
// and frees the memory occupied by the 
// event queue.
//
void fn__deinit_window_context();

//
// Pushes an event onto the global window context's event queue.
//
// Resizes the queue, whenever this operation would
// exceeds its capacity.
//
void fn__push_event(struct fn_event* ev);

//
// Pops an event from the global window context's event queue,
// and writes its contents to the variable appointed by 'ev'.
//
// If the event queue is empty, the variable appointed by 'ev'
// will be overwritten with default values.
//
void fn__pop_event(struct fn_event* ev);

//==============================================================================
// The following section declares 'callback' functions, used 
// to inform the framework about certain events.
//==============================================================================

//
// Notifies the framework that the window with index 'idx',
// was destroyed.
// 
// Generates an event with type 'fn_event_type_destroyed' and 
// releases the window's resources.
//
void fn__notify_window_destroyed(uint32_t idx);

//
// Notifies the framework that the window with index 'idx',
// was resized.
//
// Generates an event with type 'fn_event_type_resized' and
// adjusts the cache dimensions.
//
void fn__notify_window_resized(uint32_t idx, uint32_t width, uint32_t height); 

//
// Notifies the framework that the window with index 'idx'
// gained focus.
//
// Generates an event with type 'fn_event_type_focus_gained' and
// sets it as the currently focused window in the global
// context.
//
void fn__notify_window_gained_focus(uint32_t idx);

//
// Notifies the framework that the window with index 'idx'
// lost focus.
//
// Generates an event with type 'fn_event_type_focus_lost' and 
// sets it as the currently focus window in the global context.
//
void fn__notify_window_lost_focus(uint32_t idx);

//
// Notifies the framework, that a key was pressed.
//
// Generates an event with type 'fn_event_type_key_pressed' and updates 
// the internal key state.
//
void fn__notify_key_pressed(enum fn_key key, char localized_key);

//
// Notifies the framework, that a key was released.
//
// Generates an event with type 'fn_event_type_key_released' and updates
// the internal key state.
//
void fn__notify_key_released(enum fn_key key, char localized_key);

#endif  // FUNDAMENT_WINDOW_COMMON_H

