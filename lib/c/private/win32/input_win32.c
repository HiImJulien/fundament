#include "input_win32.h"
#include <fundament/event.h>
#include "../window_common.h"

#include <stdbool.h>

void fn__imp_process_keyboard_input(UINT msg, WPARAM wParam, LPARAM lParam) {
    if((msg != WM_KEYDOWN) && (msg != WM_SYSKEYDOWN) && (msg != WM_KEYUP)
       && (msg != WM_SYSKEYUP))
        return;

    const bool        is_press= (msg == WM_KEYDOWN) || (msg == WM_SYSKEYDOWN);
    const enum fn_key key     = fn__imp_map_virtual_key(wParam, lParam);
    const char        letter  = fn__imp_translate_to_char(wParam, lParam);
    if(is_press)
        fn__notify_key_pressed(key, letter);
    else
        fn__notify_key_released(key, letter);
}

enum fn_key fn__imp_map_virtual_key(WPARAM wParam, LPARAM lParam) {
    if(0x30 == wParam)
        return fn_key_0;

    if(0x31 <= wParam && wParam <= 0x39)
        return (enum fn_key) wParam - 0x31 + fn_key_a;

    if(0x41 <= wParam && wParam <= 0x5A)
        return (enum fn_key) wParam - 0x41 + fn_key_a;

    switch(wParam) {
        case VK_RETURN:
            return lParam & 0x1000000 ? fn_key_pad_enter : fn_key_enter;
        case VK_ESCAPE: return fn_key_escape;
        case VK_BACK: return fn_key_backspace;
        case VK_TAB: return fn_key_tab;
        case VK_SPACE: return fn_key_space;
        // TODO: Hyphen
        // TODO: Equal
        case VK_OEM_4: return fn_key_left_bracket;
        case VK_OEM_6: return fn_key_right_bracket;
        case VK_OEM_5: return fn_key_backslash;
        case VK_OEM_3: return fn_key_tilde;
        case VK_OEM_1: return fn_key_semicolon;
        // TODO: tick
        // TODO: grave
        case VK_OEM_COMMA: return fn_key_comma;
        case VK_OEM_PERIOD: return fn_key_dot;
        case VK_OEM_2: return fn_key_slash;
        case VK_CAPITAL: return fn_key_caps;

        case VK_F1: return fn_key_f1;
        case VK_F2: return fn_key_f2;
        case VK_F3: return fn_key_f3;
        case VK_F4: return fn_key_f4;
        case VK_F5: return fn_key_f5;
        case VK_F6: return fn_key_f6;
        case VK_F7: return fn_key_f7;
        case VK_F8: return fn_key_f8;
        case VK_F9: return fn_key_f9;
        case VK_F10: return fn_key_f10;
        case VK_F11: return fn_key_f11;
        case VK_F12: return fn_key_f12;

        case VK_PRINT: return fn_key_print;
        case VK_SCROLL: return fn_key_scroll;
        case VK_PAUSE: return fn_key_pause;
        case VK_INSERT: return fn_key_insert;
        case VK_HOME: return fn_key_home;
        case VK_PRIOR: return fn_key_page_up;
        case VK_DELETE: return fn_key_delete;
        case VK_END: return fn_key_end;
        case VK_NEXT: return fn_key_page_down;

        case VK_LEFT: return fn_key_left;
        case VK_UP: return fn_key_up;
        case VK_RIGHT: return fn_key_right;
        case VK_DOWN: return fn_key_down;

        case VK_NUMLOCK: return fn_key_pad_lock;
        case VK_DIVIDE: return fn_key_pad_div;
        case VK_MULTIPLY: return fn_key_pad_mul;
        case VK_ADD: return fn_key_pad_add;
        case VK_SUBTRACT: return fn_key_pad_sub;
        case VK_NUMPAD1: return fn_key_pad_1;
        case VK_NUMPAD2: return fn_key_pad_2;
        case VK_NUMPAD3: return fn_key_pad_3;
        case VK_NUMPAD4: return fn_key_pad_4;
        case VK_NUMPAD5: return fn_key_pad_5;
        case VK_NUMPAD6: return fn_key_pad_6;
        case VK_NUMPAD7: return fn_key_pad_7;
        case VK_NUMPAD8: return fn_key_pad_8;
        case VK_NUMPAD9: return fn_key_pad_9;
        case VK_NUMPAD0: return fn_key_pad_0;

        case VK_VOLUME_MUTE: return fn_key_mute;
        case VK_VOLUME_UP: return fn_key_vol_up;
        case VK_VOLUME_DOWN: return fn_key_vol_down;

        case VK_LCONTROL: return fn_left_ctrl;
        case VK_LSHIFT: return fn_left_shift;
        case VK_LMENU: return fn_left_alt;
        case VK_LWIN: return fn_left_system;
        case VK_RCONTROL: return fn_right_ctrl;
        case VK_RSHIFT: return fn_right_shift;
        case VK_RMENU: return fn_right_alt;
        case VK_RWIN: return fn_right_system;

        default: return fn_key_unknown;
    }
}

void fn__imp_process_mouse_input(UINT msg, LPARAM lParam) {
    const bool is_press= (msg == WM_LBUTTONDOWN) || (msg == WM_RBUTTONDOWN)
                         || (msg == WM_MBUTTONDOWN);

    enum fn_button button= 0;

    switch(msg) {
        case WM_LBUTTONDOWN: button= fn_button_left; break;

        case WM_LBUTTONUP: button= fn_button_left; break;

        case WM_RBUTTONDOWN: button= fn_button_right; break;

        case WM_RBUTTONUP: button= fn_button_right; break;

        case WM_MBUTTONDOWN: button= fn_button_middle; break;

        case WM_MBUTTONUP: button= fn_button_middle; break;
    }

    if(is_press)
        fn__notify_button_pressed(button,
                                  (int32_t) LOWORD(lParam),
                                  (int32_t) HIWORD(lParam));
    else
        fn__notify_button_released(button,
                                   (int32_t) LOWORD(lParam),
                                   (int32_t) HIWORD(lParam));
}

void fn__imp_process_mouse_move(LPARAM lParam) {
    fn__notify_mouse_moved((int32_t) LOWORD(lParam), (int32_t) HIWORD(lParam));
}

char fn__imp_translate_to_char(WPARAM wParam, LPARAM lParam) {
    BYTE state[256];
    GetKeyboardState(state);

    DWORD     ascii= 0;
    const int length=
        ToAscii(wParam, (lParam >> 16) & 0x00ff, state, (LPWORD) &ascii, 0);

    return length == 1 ? (char) ascii : 0;
}
