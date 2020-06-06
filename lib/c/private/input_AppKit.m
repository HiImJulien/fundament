#include "input_AppKit.h"
#include "window_common.h"
#include <fundament/event.h>

#import <Carbon/Carbon.h>

void fn__imp_process_keyboard_input(NSEvent* ev) {
    // ev.type is one of the NSEventTypeKey* events;
    // others are filtered out in 'fn__imp_window_poll_events'.
    const bool is_press = ev.type == NSEventTypeKeyDown;
    const enum fn_key key = fn__imp_map_virtual_key(ev.keyCode);
    fn__set_key_state(key, is_press);

    struct fn_event fev = {0, };
    fev.type = is_press ? fn_event_type_key_pressed : fn_event_type_key_released;
    fev.key = key;
    fn__push_event(&fev);
}

enum fn_key fn__imp_map_virtual_key(unsigned short keyCode) {
    switch(keyCode) {
        case kVK_ANSI_A: return fn_key_a;
        case kVK_ANSI_B: return fn_key_b;
        case kVK_ANSI_C: return fn_key_c;
        case kVK_ANSI_D: return fn_key_d;
        case kVK_ANSI_E: return fn_key_e;
        case kVK_ANSI_F: return fn_key_f;
        case kVK_ANSI_G: return fn_key_g;
        case kVK_ANSI_H: return fn_key_h;
        case kVK_ANSI_I: return fn_key_i;
        case kVK_ANSI_J: return fn_key_j;
        case kVK_ANSI_K: return fn_key_k;
        case kVK_ANSI_L: return fn_key_l;
        case kVK_ANSI_M: return fn_key_m;
        case kVK_ANSI_N: return fn_key_n;
        case kVK_ANSI_O: return fn_key_o;
        case kVK_ANSI_P: return fn_key_p;
        case kVK_ANSI_R: return fn_key_r;
        case kVK_ANSI_S: return fn_key_s;
        case kVK_ANSI_T: return fn_key_t;
        case kVK_ANSI_U: return fn_key_u;
        case kVK_ANSI_V: return fn_key_v;
        case kVK_ANSI_W: return fn_key_w;
        case kVK_ANSI_X: return fn_key_x;
        case kVK_ANSI_Y: return fn_key_y;
        case kVK_ANSI_Z: return fn_key_z;

        case kVK_ANSI_1: return fn_key_1;
        case kVK_ANSI_2: return fn_key_2;
        case kVK_ANSI_3: return fn_key_3;
        case kVK_ANSI_4: return fn_key_4;
        case kVK_ANSI_5: return fn_key_5;
        case kVK_ANSI_6: return fn_key_6;
        case kVK_ANSI_7: return fn_key_7;
        case kVK_ANSI_8: return fn_key_8;
        case kVK_ANSI_9: return fn_key_9;
        case kVK_ANSI_0: return fn_key_0;

        case kVK_Return: return fn_key_enter;
        case kVK_Escape: return fn_key_escape;
        case kVK_Delete: return fn_key_backspace;
        case kVK_Tab:    return fn_key_tab;
        case kVK_Space:  return fn_key_space;
        // TODO: hyphen
        case kVK_ANSI_Equal: return fn_key_equal;
        case kVK_ANSI_RightBracket: return fn_key_right_bracket;
        case kVK_ANSI_LeftBracket: return fn_key_left_bracket;
        case kVK_ANSI_Backslash: return fn_key_backslash;
        // TODO: tilde
        case kVK_ANSI_Semicolon: return fn_key_semicolon;
        // TODO: tick
        case kVK_ANSI_Grave: return fn_key_grave;
        case kVK_ANSI_Comma: return fn_key_comma;
        case kVK_ANSI_Period: return fn_key_dot;
        case kVK_ANSI_Slash: return fn_key_slash;
        case kVK_CapsLock: return fn_key_caps;

        case kVK_F1: return fn_key_f1;
        case kVK_F2:   return fn_key_f2;
        case kVK_F3:   return fn_key_f3;
        case kVK_F4:   return fn_key_f4;
        case kVK_F5:   return fn_key_f5;
        case kVK_F6:   return fn_key_f6;
        case kVK_F7:   return fn_key_f7;
        case kVK_F8:   return fn_key_f8;
        case kVK_F9:   return fn_key_f9;
        case kVK_F10:  return fn_key_f10;
        case kVK_F11:  return fn_key_f11;
        case kVK_F12:  return fn_key_f12;

        // TODO: print - page_down

        case kVK_LeftArrow: return fn_key_left;
        case kVK_RightArrow: return fn_key_right;
        case kVK_DownArrow: return fn_key_down;
        case kVK_UpArrow: return fn_key_up;

        default: return fn_key_unknown;
    }
}

void fn__imp_process_mouse_input(NSEvent* ev) {
    const bool is_pressed = (ev.type == NSEventTypeLeftMouseDown)
        || (ev.type == NSEventTypeRightMouseDown)
        || (ev.type == NSEventTypeOtherMouseDown);

    enum fn_button button = 0;

    switch(ev.type) {
        case NSEventTypeLeftMouseDown:
        case NSEventTypeLeftMouseUp:
            button = fn_button_left;
            break;
            
        case NSEventTypeRightMouseDown:
        case NSEventTypeRightMouseUp:
            button = fn_button_right;
            break;

        case NSEventTypeOtherMouseDown:
        case NSEventTypeOtherMouseUp:
            button = fn_button_middle;
            break;

        default: break;
    }

    fn__set_button_state(button, is_pressed);

    struct fn_event fev = {0, };
    fev.type = is_pressed ? fn_event_type_button_pressed : fn_event_type_button_released;
    
    NSWindow* win = ev.window;
    NSView* view = win.contentView;
    NSRect frame = view.frame;
    NSPoint loc = ev.locationInWindow;

    fev.x = (int32_t) loc.x;
    fev.y = (int32_t) frame.size.height - loc.y;
    fev.button = button;
    fn__push_event(&fev);
}

void fn__imp_process_mouse_wheel(NSEvent* ev) {
    struct fn_event fev = {0, };
    fev.type = fn_event_type_mouse_wheel;
    fev.mouse_wheel = ev.deltaZ;
    
}
