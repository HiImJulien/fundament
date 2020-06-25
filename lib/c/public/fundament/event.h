#ifndef FUNDAMENT_EVENT_H
#define FUNDAMENT_EVENT_H

#include <fundament/input.h>
#include <fundament/window.h>

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

struct size_event {
    uint32_t width;
    uint32_t height;
};

struct mouse_button_event {
    enum fn_button button;
    int32_t        x;
    int32_t        y;
};

struct mouse_move_event {
    int32_t x;
    int32_t y;
};

struct mouse_wheel_event {
    int32_t dt;
    int32_t x;
    int32_t y;
};

struct keyboard_event {
    enum fn_key key;
    char        letter;
};

//
// Event issued by the underlying system.
//
struct fn_event {
    enum fn_event_type type;

    // Stores the window affected.
    // For events that change the window, this property stores the window that
    // was affected by the changes.
    // For input events, this property stores the window that is currently
    // focused. Note, that the handle might be invalid, indicating that no
    // (fundament) window had focus.
    struct fn_window window;

    union {
        // Holds the new size of a window, if the event type is
        // 'fn_event_type_resized'.
        struct size_event size;

        // Holds the changed button and the position of the mouse,
        // if the event type is 'fn_event_type_button_pressed' or
        // 'fn_event_type_button_released'.
        struct mouse_button_event button;

        // Holds the new position of the mouse, if the event type is
        // 'fn_event_type_mouse_moved'.
        struct mouse_move_event mouse_move;

        // Stores the mouse wheel scroll directory the mouse's position,
        // if the event type is 'fn_event_type_mouse_wheel'.
        // The value stored in dt specifies the scroll direction.
        // A positive value means scrolling away from the user.
        // A negative value means scrolling towards the user.
        struct mouse_wheel_event mouse_wheel;

        // Holds the changed key and the letter it corresponds to,
        // if the event type is 'fn_event_type_key_pressed'
        // or 'fn_event_type_key_released'.
        struct keyboard_event key;
    };
};

#endif  // FUNDAMENT_EVENT_H
