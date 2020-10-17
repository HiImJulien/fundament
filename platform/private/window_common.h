#ifndef FUNDAMENT_WINDOW_COMMON_H
#define FUNDAMENT_WINDOW_COMMON_H

//==============================================================================
//                               WINDOW COMMON
//
// Provides functions and data structures used by both, the public API and 
// the internal implementation.
//
//==============================================================================

#include <fundament/handle_pool.h>
#include <fundament/types.h>

#include <stdint.h>

enum fn__window_state {
    fn__window_state_closed,
    fn__window_state_hidden,
    fn__window_state_visible,
    fn__window_state_maximized,
    fn__window_state_minimized
};

//
// The internal representation of a window.
//
struct fn__window {
    enum fn__window_state       state;
    uint32_t                    handle;
    fn_native_window_handle_t   native;
    uint32_t                    width;
    uint32_t                    height;
    const char*                 title;
};

#define FN_WINDOW_CAPACITY              64
#define FN_INITIAL_EVENT_CAPACITY       256

//
// Encapsulates the modules global state.
//
struct fn__window_context {
    struct fn__window       windows[FN_WINDOW_CAPACITY];  
    struct fn_handle_pool   window_pool;

    // The event queue; realized as a (growable) circular buffer.
    struct fn_event*        events;
    size_t                  events_size;
    size_t                  events_capacity;
    size_t                  events_head;
    size_t                  events_tail;
};

extern struct fn__window_context fn__g_window_context;

//==============================================================================
//                            INTERNAL EVENTS API
//
// The following section provides the internal events APIs.
//
//==============================================================================

#endif  // FUNDAMENT_WINDOW_COMMON_H

