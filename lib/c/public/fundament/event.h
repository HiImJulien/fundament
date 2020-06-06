#ifndef FUNDAMENT_EVENT_H
#define FUNDAMENT_EVENT_H

#include <fundament/window.h>
#include <fundament/input.h>

#include <stdint.h>

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
    fn_event_type_mouse_wheel,
};

//
// Event issued by the underlying system.
//
struct fn_event {
    enum fn_event_type  type;
    struct fn_window    window;

    struct {
        uint32_t    width;
        uint32_t    height;
    };

    // In case of the 'fn_event_type_button_*' event,
    // the button property stores the button that changed
    // its state.
    // E.g.: If you press the left button, then the property will
    // store 'fn_button_left'.
    //
    // In case of the 'fn_event_type_mouse_moved' event,
    // the button property stores all buttons that were pressed
    // while moving the mouse.
    struct {
        int32_t         x;
        int32_t         y;
        enum fn_button  button;
    };

    int32_t         mouse_wheel;

    char            localized_key;
    enum fn_key     key;
};

#endif  // FUNDAMENT_EVENT_H