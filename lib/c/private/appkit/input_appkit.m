#include "input_appkit.h"
#include <fundament/event.h>
#include "../window_common.h"
#include "input_key_map_appkit.h"

#include <Carbon/Carbon.h>

void fn__imp_process_keyboard_input(NSEvent* ev) {
    // ev.type is one of the NSEventTypeKey* events;
    // others are filtered out in 'fn__imp_window_poll_events'.
    const bool        is_press= ev.type == NSEventTypeKeyDown;
    const enum fn_key key     = fn__imp_map_appkit_key((uint16_t) ev.keyCode); 

    const char letter=
        ev.characters.length == 1 ? [ev.characters UTF8String][0] : 0;

    if(is_press)
        fn__notify_key_pressed(key, letter);
    else
        fn__notify_key_released(key, letter);
}

void fn__imp_process_mouse_input(NSEvent* ev) {
    const bool is_pressed= (ev.type == NSEventTypeLeftMouseDown)
                           || (ev.type == NSEventTypeRightMouseDown)
                           || (ev.type == NSEventTypeOtherMouseDown);

    enum fn_button button= 0;

    switch(ev.type) {
        case NSEventTypeLeftMouseDown:
        case NSEventTypeLeftMouseUp: button= fn_button_left; break;

        case NSEventTypeRightMouseDown:
        case NSEventTypeRightMouseUp: button= fn_button_right; break;

        case NSEventTypeOtherMouseDown:
        case NSEventTypeOtherMouseUp: button= fn_button_middle; break;

        default: break;
    }

    const NSRect  frame= ev.window.contentView.frame;
    const NSPoint loc  = ev.locationInWindow;

    const int32_t x= loc.x;
    const int32_t y= frame.size.height - loc.y;

    if(is_pressed)
        fn__notify_button_pressed(button, x, y);
    else
        fn__notify_button_released(button, x, y);
}

void fn__imp_process_mouse_wheel(NSEvent* ev) {
    NSRect  frame= ev.window.contentView.frame;
    NSPoint loc  = ev.locationInWindow;

    fn__notify_mouse_wheel_moved((int32_t) ev.deltaZ,
                                 (int32_t) loc.x,
                                 (int32_t) frame.size.height - loc.y);
}
