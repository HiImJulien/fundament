#ifndef FUNDAMENT_EVENT_H
#define FUNDAMENT_EVENT_H

//==============================================================================
//                                   EVENTS
//
// Encapsulates events raised by the underlying system.
//
//==============================================================================

#include <fundament/window.h>

//
// Enumerates known event types.
//
enum fn_event_type {
    fn_event_type_none,
    fn_event_type_closed,
    fn_event_type_resized,
    fn_event_type_focus_gained,
    fn_event_type_focus_lost,
    fn_event_type_key_pressed,
    fn_event_type_key_released,
    fn_event_type_button_pressed,
    fn_event_type_button_released,
    fn_event_type_mouse_moved,
    fn_event_type_mouse_wheel
};

//
// Represents an event.
//
struct fn_event {
    enum fn_event_type  type; 
    struct fn_window    window;
};

#endif  // FUNDAMENT_EVENT_H

